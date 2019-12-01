//#include "request.h"
//
//template<class Body, class Allocator, class Send>
//http::response<http::string_body> auth_request<Body, Allocator, Send>::get() {
//  return boost::beast::http::response<http::string_body>();
//}
//
//template<class Body, class Allocator, class Send>
//http::response<http::string_body> auth_request<Body, Allocator, Send>::put() {
//  return boost::beast::http::response<http::string_body>();
//}
//
//template<class Body, class Allocator, class Send>
//http::response<http::string_body> auth_request<Body, Allocator, Send>::post() {
//  return boost::beast::http::response<http::string_body>();
//}
//
//template<class Body, class Allocator, class Send>
//http::response<http::string_body> auth_request<Body, Allocator, Send>::delete_() {
//  return boost::beast::http::response<http::string_body>();
//}
//
//template<class Body, class Allocator, class Send>
//bool auth_request<Body, Allocator, Send>::is_auth() {
//  return false;
//}
//
//template<class Body, class Allocator, class Send>
//unsigned int auth_request<Body, Allocator, Send>::get_user_id() {
//  return user_id;
//}
//
//template<class Body, class Allocator>
//unsigned int auth_request<Body, Allocator>::get_user_id() {
//  return 0;
//}
//
//template<class Body, class Allocator>
//auth_request<Body, Allocator>::auth_request(http::request<Body> req) : req(req){
//
//}
//
//template<class Body, class Allocator>
//void auth_request<Body, Allocator>::update_token() {
//
//}
