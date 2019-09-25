
String getHttpGETRequestWithUrlAndHost(String host, String url) {
  return String("GET ") + url + " HTTP/1.1\r\n" +
         "Host: " + host + "\r\n" +
         "Connection: close\r\n\r\n";
}
