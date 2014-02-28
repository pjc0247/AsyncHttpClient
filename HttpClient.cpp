#include "HttpClient.h"

using namespace std;

static queue<Request*> requestQ;
static queue<Response*> responseQ;
static pthread_mutex_t requestMutex, responseMutex, signalMutex;
static pthread_cond_t signalCond;
static bool quit = false;

static size_t writeData(void *ptr, size_t size, size_t nmemb, void *stream){
    string *recvBuffer = (string*)stream;
    size_t sizes = size * nmemb;
    
    recvBuffer->append(
		(char*)ptr, 0, sizes);
    
    return sizes;
}
static size_t writeHeaderData(void *ptr, size_t size, size_t nmemb, void *stream){
    string *recvBuffer = (string*)stream;
    size_t sizes = size * nmemb;
    
    recvBuffer->append(
		(char*)ptr, 0, sizes);
    
    return sizes;
}
static void* worker(void *arg){

	while( !quit ){
		Request *request = NULL;

		pthread_cond_wait( &signalCond, &signalMutex );

		pthread_mutex_lock( &requestMutex );
			if( !requestQ.empty() ){
				request = requestQ.front();
				requestQ.pop();
			}
		pthread_mutex_unlock( &requestMutex );

		if( request != NULL ){
			CURL *curl;
			Response *response = new Response();

			response->setRequest( request );

			curl = curl_easy_init();

			curl_easy_setopt( curl, CURLOPT_URL, request->getUrl().c_str() );

			curl_easy_setopt( curl, CURLOPT_WRITEFUNCTION, writeData);
			curl_easy_setopt( curl, CURLOPT_WRITEDATA, &response->getBody());
			curl_easy_setopt( curl, CURLOPT_HEADERFUNCTION, writeHeaderData);
			curl_easy_setopt( curl, CURLOPT_HEADERDATA, &response->getHeader());

			switch( request->getType() ){
			case HttpGet:
				break;
			case HttpPost:
				curl_easy_setopt( curl, CURLOPT_POST, true);
				curl_easy_setopt( curl, CURLOPT_POSTFIELDS, request->getData().c_str() );
				curl_easy_setopt( curl, CURLOPT_POSTFIELDSIZE, request->getData().length() );
				break;
			}

			curl_easy_perform( curl );

			curl_easy_cleanup( curl );

			pthread_mutex_lock( &responseMutex );
				responseQ.push( response );
			pthread_mutex_unlock( &responseMutex );
		}
	}

	return NULL;
}


HttpClient *HttpClient::instance = NULL;

HttpClient *HttpClient::getInstance(){
	if( instance == NULL ){
		instance = new HttpClient();
	}

	return instance;
}

HttpClient::HttpClient(){
	pthread_t tid;

	pthread_mutex_init( &requestMutex, 0 );
	pthread_mutex_init( &responseMutex, 0 );
	pthread_mutex_init( &signalMutex, 0 );
	pthread_cond_init( &signalCond, 0 );

	pthread_create( &tid,0, worker, 0 );
}
HttpClient::~HttpClient(){
	quit = true;

	pthread_mutex_destroy( &requestMutex );
	pthread_mutex_destroy( &responseMutex );
	pthread_mutex_destroy( &signalMutex );
	pthread_cond_destroy( &signalCond );
}


bool HttpClient::requestGet(
	const char *url,ResponseCallback cb){

	pthread_mutex_lock( &requestMutex );

		Request *request = new Request();

		request->setType( HttpGet );
		request->setUrl( string(url) );
		request->setCallback( cb );
		requestQ.push( request );

	pthread_mutex_unlock( &requestMutex );

	pthread_cond_signal( &signalCond );
	return true;
}
bool HttpClient::requestPost(
	const char *url,const char *data,
	ResponseCallback cb){

	pthread_mutex_lock( &requestMutex );

		Request *request = new Request();

		request->setType( HttpPost );
		request->setUrl( string(url) );
		request->setData( string(data) );
		request->setCallback( cb );
		requestQ.push( request );

	pthread_mutex_unlock( &requestMutex );

	pthread_cond_signal( &signalCond );
	return true;
}

bool HttpClient::poll(){
	while( true ){
		pthread_mutex_lock( &responseMutex );
			if( responseQ.empty() )
				break;
				
			Response *response = responseQ.front();
			Request *request = response->getRequest();
	
			responseQ.pop();
		pthread_mutex_unlock( &responseMutex );
		
		
		request->getCallback()
			( response->getBody() );

		delete response;
		delete request;
	}
	
	return true;
}
