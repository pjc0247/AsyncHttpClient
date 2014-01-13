#include ".h"

using namespace std;

Response::Response(){
}
Response::~Response(){
}
string &Response::getHeader(){
	return header;
}
string &Response::getBody(){
	return body;
}
void Response::setRequest(Request *request){
	this->request = request;
}
Request *Response::getRequest(){
	return request;
}
