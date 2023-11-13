## http-cpp Library (23-11-13 수정중)

##### 왜 C\+\+는 HTTP 통신을 지원하지 않는거죠? C++20 에는 STL로 지원해줄 법도 하지 않나요?
오픈소스 라이브러리(Boost,cpp-httplib,cURL) 전부 실패해서 직접 만들었음.

기본 골자는 쓰레드 하나를 할애해서 tcp 소켓으로 접속해서 필요할때마다 클라이언트로 접속시켜서 http 통신을 진행한다.

요청하고 받는 로직은 구현했지만, 사실 웹서버와 쌍방향 통신도 가능하다. 

나는 잘쓰고 있지만... 라이브러리화하는 과정에서 생긴 런타임 라이브러리 오류 해결중ㅜ
   
### ✨ Fuction Description
   
   
-<code>Server obj(Address:Port)</code> : 도메인 주소로 변환해야합니다.

  <pre>
<code>Server myServer("127.0.0.1:8080");
</code>
</pre>
  
  
-<code>RegisterHandler("/test","http method",function)</code> :   웹서버에서 보낸 response를 미리 핸들러에 등록해놔야한다.  
  
  <pre>
<code>RegisterHandler("/test", "http method", function)
</code>
</pre>

  - <code>"/test"</code> : 핸들러를 등록할 경로입니다. 클라이언트가 이 경로로 요청을 보내면 등록된 함수가 실행됩니다.
  - <code>"http method"</code> : HTTP 메서드로, 클라이언트가 사용하는 메서드를 지정합니다. 예를 들어, "GET" 요청에 필요한 데이터를 나타내는 문자열입니다. (예: 쿼리 매개변수 등)
  - <code>fuction</code> : 데이터의 크기를 나타내는 변수입니다
   
   
-<code>SendGET("/example", data, sizeof(data))</code> : 성공시 0, 실패시 -1를 반환
  <pre>
<code>SendGET("/example", data, sizeof(data))
</code>
</pre>

  - <code>path</code> : GET 요청을 보낼 리소스의 경로를 나타내는 문자열입니다.
  - <code>data</code> : GET 요청에 필요한 데이터를 나타내는 문자열입니다. (예: 쿼리 매개변수 등)
  - <code>dataSize</code> : 데이터의 크기를 나타내는 변수입니다


   
   
- <code>SendPOST("/example", data, sizeof(data))</code> : 성공시 0, 실패시 -1를 반환
   <pre>
<code>SendPOST("/example", data, sizeof(data))
</code>
</pre>   

  - <code>path</code> : POST 요청을 보낼 리소스의 경로를 나타내는 문자열입니다.
  - <code>data</code> : POST 요청에 필요한 데이터를 나타내는 문자열입니다. (예: 쿼리 매개변수 등)
  - <code>dataSize</code> : 데이터의 크기를 나타내는 변수입니다
    

   
   
   
* * *
   
   
### ✨ example code

<pre><code>
#include<iostream>
#include "http-cpp.h"
#pragma comment(lib,"http-cpp.lib")
   
   std::string test(const std::string& params) {
    return "Hello, this is the response from the server!";
}

int main() {
    Server myServer("127.0.0.1:8080");

    myServer.RegisterHandler("/hello", "GET", test);

    std::string data = "GET request data";
    int resultGET = Server::SendGET("/example", data, sizeof(data));

    if (resultGET == 0)
        std::cout << "GET request sent successfully." << std::endl;
    else 
        std::cerr << "Failed to send GET request." << std::endl;
    return 0;
}
</code>
</pre>

