package model.stocks

import com.google.gson.annotations.SerializedName

data class StockResponse(
    @SerializedName("symbol") val symbol: String,
    @SerializedName("open") val open: Float,
    @SerializedName("high") val high: Float,
    @SerializedName("low") val low: Float,
    @SerializedName("close") val close: Float,
    @SerializedName("volume") val volume: Float
)