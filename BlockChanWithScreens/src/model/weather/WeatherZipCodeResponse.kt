package com.blockchainwithscreens.model.weather

import com.google.gson.annotations.SerializedName

data class WeatherZipCodeResponse(
    @SerializedName("city_name") val cityName: String,
    @SerializedName("main_description") val mainDescription: String,
    @SerializedName("secondary_description") val secondaryDescription: String,
    @SerializedName("icon_url") val iconUrl: String,
    @SerializedName("current_temp") val currentTemp: Float,
    @SerializedName("temp_min") val tempMin: Float,
    @SerializedName("temp_max") val tempMax: Float
)