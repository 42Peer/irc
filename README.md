# pseudo_code

```cpp
struct user_data {
	_user_name
	_socket_id
	_nick_name
	_channel_lists
	_validation
};

class Server {
public:
	Join()
	Delete()
private:
	Channel* _channel_lists
	_user_list 
	_password
}

class Channel {
public:
	addUser()
	deleteUser()
private:
	_channel_name
	_user_list
}


class Handler {
public:
	run() {
		// setKevent()
		// (recv 이벤트 발생시 데이터가 존재할때, 루프로) callParse() 
		// -> parsing() 
		// -> is_correct() 
		// -> figureCmd()
	}
private:
	_kq
	_event_list
	_monitor
	_server

	callParse()
}

setKevent()

```