const MAX_SIZE = 256;
const MAX_MD5 = 33;

struct response{
	string msg<MAX_SIZE>;
	string md5<MAX_MD5>;
};
typedef struct response response;

program STORE_SERVICE{
	version STORE_VERSION{
		void init()				= 1;
		int store(string sender<MAX_SIZE>, string receiver<MAX_SIZE>,
			unsigned int msg_id, string msg<MAX_SIZE>, string md5<MAX_MD5>)	= 2;
		int getNumMessages(string user<MAX_SIZE>)	= 3;
		int getMessage(string user<MAX_SIZE>, unsigned int msg_id, response *res)	= 4;
	} = 1;
} = 666;