AsyncHttpClient
===============

async http client


Example
------
```C++
http = HttpClient::getInstance();

http->requestGet("http://github.com", [](std::string response){
    printf("response : %s\n", response.c_str());
  });
  
while( true ){ // main loop
  http->poll();
}
```
