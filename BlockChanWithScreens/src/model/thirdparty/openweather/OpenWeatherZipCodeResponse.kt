package com.blockchainwithscreens.model

import com.blockchainwithscreens.model.openweather.Weather
import com.blockchainwithscreens.model.openweather.WeatherMain

data class OpenWeatherZipCodeResponse (
    val name: String,
    val weather: List<Weather>,
    val main: WeatherMain
)