#include "HttpClient.h"

using namespace std;

Request::Request(){
}
Request::~Request(){
}
void Request::setUrl(string &url){
	this->url.assign( url );
}
void Request::setData(string &data){
	this->data.assign( data );
}
string &Request::getUrl(){
	return url;
}
string &Request::getData(){
	return data;
}

void Request::setCallback(ResponseCallback cb){
	this->callback = cb;
}
ResponseCallback Request::getCallback(){
	return callback;
}

void Request::setType(RequestType type){
	this->type = type;
}
RequestType Request::getType(){
	return type;
}
