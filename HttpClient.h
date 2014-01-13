#ifndef _HTTP_CLIENT_
#define _HTTP_CLIENT_

#include <string>
#include <vector>
#include <queue>
#include <functional>

#include <curl/curl.h>
#include <pthread/pthread.h>

typedef std::function<void (std::string&)> ResponseCallback;
#define ResponseHandler(a) [](std::string &a)

enum RequestType{
	HttpGet = 1,
	HttpPost
};

class Request{
public:

	Request();
	virtual ~Request();
	
	void setUrl(std::string &url);
	void setData(std::string &data);

	std::string &getUrl();
	std::string &getData();

	void setCallback(ResponseCallback cb);
	ResponseCallback getCallback();

	void setType(RequestType type);
	RequestType getType();
protected:
	std::string url;
	std::string data;

	ResponseCallback callback;

	RequestType type;
};
class Response{
public:
	Response();
	virtual ~Response();

	std::string &getHeader();
	std::string &getBody();

	void setRequest(Request *request);
	Request *getRequest();

protected:
	std::string header;
	std::string body;

	Request *request;
};

class HttpClient{
public:
	static HttpClient *getInstance();
private:
	static HttpClient *instance;

	HttpClient();
	virtual ~HttpClient();

public:
	bool requestGet(
		const char *url, ResponseCallback cb);
	bool requestPost(
		const char *url, const char *data,
		ResponseCallback cb);

	
	bool poll();
};

#endif // _HTTP_CLIENT_
