package com.blockchainwithscreens

import com.blockchainwithscreens.model.OpenWeatherZipCodeResponse
import com.blockchainwithscreens.model.Secrets
import com.blockchainwithscreens.model.weather.WeatherZipCodeResponse
import com.google.gson.Gson
import com.google.gson.JsonParser
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
import io.ktor.http.ContentDisposition.Companion.File
import io.ktor.http.content.files
import io.ktor.http.content.static
import io.ktor.http.content.staticRootFolder
import kotlinx.coroutines.*
import model.stocks.StockResponse
import model.thirdparty.alphavantage.MetaData
import model.thirdparty.alphavantage.TimeSeriesDaily
import java.io.File
import java.util.concurrent.TimeUnit

fun main(args: Array<String>): Unit = io.ktor.server.netty.EngineMain.main(args)

// This simple cache is mapping from the zipcode passed in as a param to the response from the source
val weatherZipCodeCache = HashMap<String, WeatherZipCodeResponse>()
// Keeps track of the last time the data for the weather at a zip code was updated
val weatherZipCodeLastUpdate = HashMap<String, Long>()

val stocksCache = HashMap<String, StockResponse>()
val stocksLastUpdate = HashMap<String, Long>()

val gson = Gson()

@Suppress("unused") // Referenced in application.conf
@kotlin.jvm.JvmOverloads
fun Application.module(testing: Boolean = false) {
    val client = HttpClient(Apache) {
        install(JsonFeature) {
            serializer = GsonSerializer()
        }
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
                val openWeatherResponse = client.get<String>(url)
                val weather = gson.fromJson(openWeatherResponse, OpenWeatherZipCodeResponse::class.java)
                val weatherDescriptions = weather.weather[0]
                val weatherMain = weather.main
                val iconUrl = "https://blockchainwithscreens.herokuapp.com/weather/image/" + weatherDescriptions.icon + ".jpg"
                val response = WeatherZipCodeResponse(
                    weather.name,
                    weatherDescriptions.main,
                    weatherDescriptions.description,
                    iconUrl,
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
                val symbol = call.parameters["symbol"] ?: return@get

                val lastUpdate = stocksLastUpdate[symbol]
                if (lastUpdate != null) {
                    if (System.currentTimeMillis() - lastUpdate < TimeUnit.MINUTES.toMillis(1)) {
                        val response = stocksCache[symbol]
                        call.respondText { gson.toJson(response) }
                        return@get
                    }
                }

                val url = "https://www.alphavantage.co/query?function=TIME_SERIES_DAILY&symbol=$symbol&apikey=${Secrets.ALPHAV_API_KEY}"
                val alphaVantageResponse = client.get<String>(url)
                val jsonElement = JsonParser()
                val stockJsonObject = jsonElement.parse(alphaVantageResponse)?.asJsonObject
                if (stockJsonObject == null) {
                    // error out here
                }

                val metadataJsonObject = stockJsonObject?.getAsJsonObject("Meta Data")
                val metaData = gson.fromJson<MetaData>(metadataJsonObject, MetaData::class.java)
                val lastRefreshed = metaData.lastRefreshed
                val refreshDate = lastRefreshed.substringBefore(" ")

                val timeSeriesJsonObject = stockJsonObject?.getAsJsonObject("Time Series (Daily)") ?: return@get
                val timeSeriesRefreshDateJsonObject = timeSeriesJsonObject[refreshDate]
                val timeSeries = gson.fromJson<TimeSeriesDaily>(timeSeriesRefreshDateJsonObject, TimeSeriesDaily::class.java)

                val open = timeSeries.open.toFloat()
                val high = timeSeries.high.toFloat()
                val low = timeSeries.low.toFloat()
                val close = timeSeries.close.toFloat()
                val volume = timeSeries.volume.toFloat()

                val response = StockResponse(
                    metaData.symbol,
                    open,
                    high,
                    low,
                    close,
                    volume
                )

                stocksLastUpdate[symbol] = System.currentTimeMillis()
                stocksCache[symbol] = response
                call.respondText { gson.toJson(response) }
            }
        }
        static("/weather") {
            staticRootFolder = File("weather")
            static("/image") {
                files("image")
            }
        }
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
