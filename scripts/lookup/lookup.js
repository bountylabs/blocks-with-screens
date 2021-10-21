#!/usr/bin/env node
import { execSync } from "child_process";
import fetch from "node-fetch";
import fs from "fs";
import path from "path";
import { execPath } from "process";

// curl 'https://api.twitter.com/graphql' \
//   -H 'x-csrf-token: e84f85ee9684298bdaeaa04324409a600f7bd366d918a4c7a76c4f5c91ac428ca3e07199a3c2903fce8f5c400b0b1a50035eb06e6861053eff5ff3be01e49d163f95fdbb78dc839908dc11cde508ae69' \
//   -H 'authorization: <AUTHORIZATION_BEARER>' \
//   -H 'content-type: application/json' \
//   -H 'cookie: <COOKIE>' \
//   --data-raw '{"query":"query {\n  user: user_result_by_rest_id(rest_id: \"23604692\", safety_level: FilterAll) {\n    ...UserResults\n  }\n}\n\nfragment UserResults on UserResults {\n  result {\n    __typename\n    ... on User {\n      ...User\n    }\n  }\n}\n\nfragment User on User {\n  id\n  rest_id\n  legacy {\n    ...ApiUser\n  }\n}\n\nfragment ApiUser on ApiUser {\n  screen_name\n}","variables":null}' \
//   --compressed \
//   | jq ".data.user.result.legacy.screen_name"
async function twitterUsernameById(userId) {
  const { authorization, cookie } = readAuthSync();

  const headers = {
    "x-csrf-token":
      "e84f85ee9684298bdaeaa04324409a600f7bd366d918a4c7a76c4f5c91ac428ca3e07199a3c2903fce8f5c400b0b1a50035eb06e6861053eff5ff3be01e49d163f95fdbb78dc839908dc11cde508ae69",
    authorization,
    "content-type": "application/json",
    cookie,
  };

  const response = await fetch("https://api.twitter.com/graphql", {
    headers,
    body: `{"query":"query {\\n  user: user_result_by_rest_id(rest_id: \\"${userId}\\", safety_level: TweetDetail) {\\n    ...UserResults\\n  }\\n}\\n\\nfragment UserResults on UserResults {\\n  result {\\n    __typename\\n    ... on User {\\n      ...User\\n    }\\n  }\\n}\\n\\nfragment User on User {\\n  id\\n  rest_id\\n  legacy {\\n    ...ApiUser\\n  }\\n}\\n\\nfragment ApiUser on ApiUser {\\n  screen_name\\n}","variables":null}`,
    method: "POST",
    mode: "cors",
  });

  const json = await response.json();

  try {
    return json.data.user.result.legacy.screen_name;
  } catch (err) {
    // throw new Error(`[userid=${userId}] [${JSON.stringify(json)}]`);
    return null;
  }
}

// Twitter Employee Service
// see https://docbird.twitter.biz/employeeservice-api/employee_api.html
// curl "https://employeeservice-api.twitter.biz/employees.json?q=last_name:jorgensen&q=first_name:noah" \
//   | jq
async function employeeQuery({ id, firstName, lastName }) {
  const endpoint = "https://employeeservice-api.twitter.biz/employees.json";

  const queryParams = [];

  if (id) {
    queryParams.push(`q=id:${id}`);
  } else {
    queryParams.push(
      `q=first_name:${firstName.toLowerCase()}`,
      `q=last_name:${lastName.toLowerCase()}`
    );
  }

  const requestUrl = [endpoint, queryParams.join("&")].join("?");

  const response = await fetch(requestUrl);

  const userList = await response.json();

  const filteredUserList = userList.filter(
    (u) => u.admin_twitter_id || u.twitter_id
  );

  if (filteredUserList.length === 1) {
    const [user] = filteredUserList;
    return user;
  }

  const args = JSON.stringify({ firstName, lastName });

  if (!filteredUserList.length) {
    console.error({ userList });
    throw new Error(
      `[employeeQuery(${args})] No users found with admin_twitter_id`
    );
  }
  if (filteredUserList.length > 1) {
    // every user has the same username (i.e. it is the same person)
    const sameUsernames = everyFieldMatches(filteredUserList, "username");
    // every user has the same admin_twitter_id (i.e. it is the same person)
    const sameAdminTwitterIds = everyFieldMatches(
      filteredUserList,
      "admin_twitter_id"
    );

    if (!sameUsernames || !sameAdminTwitterIds) {
      console.error({ userList });
      throw new Error(
        `[employeeQuery(${args})] Multiple users found, need to disambiguate`
      );
    }

    const [user] = filteredUserList;
    return user;
  }
}

function everyFieldMatches(itemList, field) {
  // every item has the same item[field] value
  const fieldList = itemList.map((_) => _[field]);
  let [lastField] = fieldList;
  for (let field of fieldList) {
    if (lastField !== field) {
      return false;
    }
  }

  return true;
}

async function getBlockParams({ id, firstName, lastName }) {
  // console.debug(
  //   `getBlockParams[${JSON.stringify({ id, firstName, lastName })}]`
  // );

  const today = Date.now();

  const employee = await employeeQuery({ id, firstName, lastName });

  // "202109130800Z"
  const year = employee.start_date.substr(0, 4);
  const month = employee.start_date.substr(4, 2);
  const day = employee.start_date.substr(6, 2);
  const startDate = new Date([month, day, year].join("-"));
  const age = today - startDate;
  const ageYears = Math.floor(age / MS.YEAR);

  // console.warn({ employee });
  // console.warn({ year, month, day, startDate, age, ageYears });

  // if (ageYears < 5) {
  //   const debug = JSON.stringify({ employee });
  //   throw new Error(
  //     `Employee age [${ageYears} years] is lower than ${AGE_YEARS_THRESHOLD} year requirement [employee=${debug}]`
  //   );
  // }

  const twitterUserId = employee.admin_twitter_id || employee.twitter_id;
  const username = await twitterUsernameById(twitterUserId);
  const ldap = employee.username;

  return { ldap, username, ageYears };
}

function readAuthSync() {
  const [authorization, cookie] = fs
    .readFileSync(path.resolve(PATHS.inputDir, "auth"), "utf8")
    .split("\n");
  return { authorization, cookie };
}

function sourceMainWithParams(sourceMainLines, params) {
  const outputLines = [];
  for (let line of sourceMainLines) {
    if (line.match(RE.sourceConstUsername)) {
      const name = params.username ? `@${params.username}` : params.ldap;
      outputLines.push(`const char *username = "${name}";`);
    } else if (line.match(RE.sourceConstYears)) {
      const name = params.username ? `@${params.username}` : params.ldap;
      outputLines.push(`const char *years = "${params.ageYears} years";`);
    } else {
      outputLines.push(line);
    }
  }

  return outputLines.join("\n");
}

async function main() {
  try {
    // clear previous output
    execSync(`rm -rf ${path.resolve(PATHS.outputDir, "*")}`);

    const namesList = fs
      .readFileSync(path.resolve(PATHS.inputDir, "names.txt"), "utf8")
      .split("\n");

    const promiseBlockParams = [];

    for (let name of namesList) {
      const match = name.match(RE.namesParser);
      if (!match) {
        continue;
      }
      const [, id, firstName, lastName] = match;
      const promise = getBlockParams({ id, firstName, lastName });
      promiseBlockParams.push({ name, promise });
    }

    await Promise.all(promiseBlockParams.map((p) => p.promise));

    const sourceMainLines = fs
      .readFileSync(PATHS.sourceMain, "utf8")
      .split("\n");

    // walk each params and output to filesystem
    for (let blockParams of promiseBlockParams) {
      const outputFile = blockParams.name;
      const params = await blockParams.promise;
      const outputSourceMain = sourceMainWithParams(sourceMainLines, params);

      fs.writeFileSync(
        path.resolve(PATHS.outputDir, outputFile),
        outputSourceMain,
        "utf8"
      );

      console.info({ outputFile, params });
      execSync(`open ${PATHS.outputDir}`);
    }
  } catch (err) {
    console.error("[main]", err);
  }
}

const PATHS = {};
PATHS.inputDir = new URL("input", import.meta.url).pathname;
PATHS.outputDir = new URL("output", import.meta.url).pathname;
PATHS.sourceMain = new URL(
  "../../blockware/fireworks/src/main.cpp",
  import.meta.url
).pathname;

const AGE_YEARS_THRESHOLD = 5;

const MS = {};
MS.SECOND = 1000;
MS.MINUTE = 60 * MS.SECOND;
MS.HOUR = 60 * MS.MINUTE;
MS.DAY = 24 * MS.HOUR;
MS.YEAR = 365 * MS.DAY;

const RE = {};
RE.namesParser = /^(\d+)?\s?([^ ]+) (.*)$/;
RE.sourceConstUsername = /^const char \*username = /;
RE.sourceConstYears = /^const char \*years = /;

main();
