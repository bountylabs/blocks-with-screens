package com.blockchainwithscreens

import com.blockchainwithscreens.model.OpenWeatherZipCodeResponse
import com.blockchainwithscreens.model.Secrets
import com.blockchainwithscreens.model.weather.WeatherZipCodeResponse
import com.google.gson.Gson
import io.ktor.application.*
import io.ktor.response.*
import io.ktor.routing.*
import io.ktor.http.*
import kotlinx.html.*
import kotlinx.css.*
import io.ktor.client.*
import io.ktor.client.engine.apache.*
import io.ktor.client.features.json.*
import io.ktor.client.request.*
import io.ktor.features.ContentNegotiation
import kotlinx.coroutines.*
import java.util.concurrent.TimeUnit

fun main(args: Array<String>): Unit = io.ktor.server.netty.EngineMain.main(args)

// This simple cache is mapping from the zipcode passed in as a param to the response from the source
val weatherZipCodeCache = HashMap<String, WeatherZipCodeResponse>()

// Keeps track of the last time the data for the weather at a zip code was updated
val weatherZipCodeLastUpdate = HashMap<String, Long>()

val gson = Gson()

@Suppress("unused") // Referenced in application.conf
@kotlin.jvm.JvmOverloads
fun Application.module(testing: Boolean = false) {
    val client = HttpClient(Apache) {
        install(JsonFeature) {
            serializer = GsonSerializer()
        }
    }
    install(ContentNegotiation) {
    }
    runBlocking {
        // Sample for making a HTTP Client request
        /*
        val message = client.post<JsonSampleClass> {
            url("http://127.0.0.1:8080/path/to/endpoint")
            contentType(ContentType.Application.Json)
            body = JsonSampleClass(hello = "world")
        }
        */
    }

    routing {
        get("/") {
            call.respondText("HELLO WORLD!", contentType = ContentType.Text.Plain)
        }
        route("/weather") {
            get("/zip/{zipcode}") {
                // Make call to open weather API
                val zipcodeParam = call.parameters["zipcode"] ?: return@get

                val lastUpdate = weatherZipCodeLastUpdate[zipcodeParam]
                if (lastUpdate != null) {
                    if (System.currentTimeMillis() - lastUpdate < TimeUnit.MINUTES.toMillis(1)) {
                        val response = weatherZipCodeCache[zipcodeParam]
                        call.respondText { gson.toJson(response) }
                        return@get
                    }
                }

                val url = "http://api.openweathermap.org/data/2.5/weather?zip=$zipcodeParam,us&appid=${Secrets.OPEN_WEATHER_API_KEY}"
                val tempClient = HttpClient()
                val openWeatherResponse = tempClient.get<String>(url)
                val weather = gson.fromJson(openWeatherResponse, OpenWeatherZipCodeResponse::class.java)
                val weatherDescriptions = weather.weather[0]
                val weatherMain = weather.main
                val response = WeatherZipCodeResponse(
                    weather.name,
                    weatherDescriptions.main,
                    weatherDescriptions.description,
                    weatherDescriptions.icon,
                    weatherMain.temp,
                    weatherMain.temp_min,
                    weatherMain.temp_max
                )

                weatherZipCodeLastUpdate[zipcodeParam] = System.currentTimeMillis()
                weatherZipCodeCache[zipcodeParam] = response
                call.respondText { gson.toJson(response) }
            }
        }
        route("/stocks") {
            get("/symbol/{symbol}") {
                // Make call to alpha vantage, TIME_SERIES_DAILY
                // Parse "Meta Data" and "Times Series (Daily)" for the lastest day, the close price always represents the most up to date price
            }
        }


//        get("/html-dsl") {
//            call.respondHtml {
//                body {
//                    h1 { +"HTML" }
//                    ul {
//                        for (n in 1..10) {
//                            li { +"$n" }
//                        }
//                    }
//                }
//            }
//        }
//
//        get("/styles.css") {
//            call.respondCss {
//                body {
//                    backgroundColor = Color.red
//                }
//                p {
//                    fontSize = 2.em
//                }
//                rule("p.myclass") {
//                    color = Color.blue
//                }
//            }
//        }
    }
}

data class JsonSampleClass(val hello: String)

fun FlowOrMetaDataContent.styleCss(builder: CSSBuilder.() -> Unit) {
    style(type = ContentType.Text.CSS.toString()) {
        +CSSBuilder().apply(builder).toString()
    }
}

fun CommonAttributeGroupFacade.style(builder: CSSBuilder.() -> Unit) {
    this.style = CSSBuilder().apply(builder).toString().trim()
}

suspend inline fun ApplicationCall.respondCss(builder: CSSBuilder.() -> Unit) {
    this.respondText(CSSBuilder().apply(builder).toString(), ContentType.Text.CSS)
}
