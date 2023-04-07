#include "Parsing.h"
#include <map>
#include <vector>
#include <iostream>
#include <string>


//__inline std::map<int, std::ve emptystring(void);
// 
// msgcase(buf);

std::map<int, std::vector<std::string > > pasing(std::string buf)
{
	std::vector<std::string> ret_vector;
	std::map<int, std::vector<std::string> > ret_map;
	if (buf == "")
	{
		ret_vector.push_back("");
		ret_map[-3] = ret_vector;
		return (ret_map);
	}
	else if (buf[0] != '/')
	{
		ret_vector.push_back(buf);
		ret_map[0] = ret_vector;
		return (ret_map);
	}
	else
	{
		// 명령어 파싱 -> invalid 경우에는 return 
	
		// (1) / 뒤부터 화이트스페이스까지 끊어온다.
		// (2) 그리고 1~11에 맞는게 있는지 체크
		// (2-1) 맞는게 있다면 리턴
		// (2-2) 맞는게 없다면 에러다 에러 리턴	
	}

}

/* 
	핸들러에서 파싱함수에게 아래와같이 넘겨준다.
	string buf로 할당되어있고,
	read (socket, buf, ) 하게되고
	parsing ( buf ) 로 받게된다.
	
	여기서 
*/