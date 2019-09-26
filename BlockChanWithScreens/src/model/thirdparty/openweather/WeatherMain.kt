package com.blockchainwithscreens.model.openweather

data class WeatherMain(val temp: Float,
                       val pressure: Int,
                       val humidity: Int,
                       val temp_min: Float,
                       val temp_max: Float)