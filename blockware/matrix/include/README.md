# Why is this folder here?

`blockware/matrix/.vscode/c_cpp_properties.json` keeps including `"blockware/matrix/include",` in the `includePath` array under the `PlatformIO` entry in `configurations`. So we create it to get rid of an annoying error in VSCode

```
⚠️ Cannot find "/Users/noah/github/blocks-with-screens/blockware/matrix/include".
```
