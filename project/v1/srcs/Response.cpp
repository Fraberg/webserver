#include "../includes/Headers.hpp"

/*
** constructors / destructors
*/

Response::Response(void)
{
    init();
}

/*
** private class methods
*/

void Response::init(void)
{
    // Status Line
    _http_version.clear();
    _status_code = 0;
    _reason_phrase.clear();
    _allow.clear();
    _content_language.clear();
    _content_length = 0;
    _content_location.clear();
    _content_type.clear();
    _last_modified.clear();
    _location.clear();
    _date.clear();
    _retry_after = -1;
    _server.clear();
    _transfer_encoding.clear();
    _www_authenticate.clear();
    // Request body
    _body.clear();
    // Content to write in fd
    _to_send.clear();
    // Other headers
    _keep_alive.clear();
    // Other
    _bytes_send = 0;
    send_status = HANDLE_RESPONSE;
    read_fd = -1;
}

// int Response::concat_to_send(void)
// {
//     size_t i;

//     // 1. Status Line
//     _to_send += _http_version + " ";
//     _to_send += std::to_string(_status_code) + " ";
//     _to_send += _reason_phrase + "\r\n";

//     // 2. Request Headers, dans l'ordre du sujet
//     if (_content_length >= 0)
//     {
//         _to_send += "Content-Length: " + std::to_string(_content_length) + "\r\n";
//     }

//     if (!_allow.empty())
//     {
//         _to_send += "allow: " + _allow + "\r\n";
//     }
//     if (!_last_modified.empty())
//     {
//         _to_send += "Last-Modified: " + _last_modified + "\r\n";
//     }
//     if (_retry_after >= 0)
//     {
//         _to_send += "Retry-After: " + std::to_string(_retry_after) + "\r\n";
//     }
//     if (!_location.empty() && _status_code == 201)
//     {
//         _to_send += "Location: " + _location + "\r\n";
//     }
//     if (!_date.empty())
//     {
//         _to_send += "Date: " + _date + "\r\n";
//     }
//     if (!_server.empty())
//     {
//         _to_send += "Server: " + _server + "\r\n";
//     }
//     if (!_transfer_encoding.empty())
//     {
//         _to_send += "Transfer-Encoding: " + _transfer_encoding + "\r\n";
//     }
//     if (!_content_type.empty())
//     {
//         _to_send += "Content-Type:";
//         i = 0;
//         while (i < _content_type.size())
//         {
//             _to_send += " " + _content_type[i];
//             if (i + 1 < _content_type.size())
//                 _to_send += ";";
//             i++;
//         }
//         _to_send += "\r\n";
//     }

//     // 3. Request body
//     _to_send += "\r\n";
//     if (!_body.empty())
//     {
//         _to_send += _body;
//         LOG_WRT(Logger::DEBUG, "BODY EMPTY IN RESPONSE");
//     }
//     return (1);
// }


int Response::concat_to_send(void)
{
    size_t i;

    // 1. Status Line
    _to_send.append(_http_version);
    _to_send.append(" ");
    _to_send.append(std::to_string(_status_code));
    _to_send.append(" ");
    _to_send.append(_reason_phrase);
    _to_send.append("\r\n");

    // 2. Request Headers, dans l'ordre du sujet
    if (_content_length >= 0)
    {
        _to_send.append("Content-Length: ");
        _to_send.append(std::to_string(_content_length));
        _to_send.append("\r\n");
    }

    if (!_allow.empty())
    {
        _to_send.append("allow: ");
        _to_send.append(_allow);
        _to_send.append("\r\n");
    }
    if (!_last_modified.empty())
    {
        _to_send.append("Last-Modified: ");
        _to_send.append(_last_modified);
        _to_send.append("\r\n");
    }
    if (_retry_after >= 0)
    {
        _to_send.append("Retry-After: ");
        _to_send.append(std::to_string(_retry_after));
        _to_send.append("\r\n");
    }
    if (!_location.empty() && _status_code == 201)
    {
        _to_send.append("Location: ");
        _to_send.append(_location);
        _to_send.append("\r\n");
    }
    if (!_date.empty())
    {
        _to_send.append("Date: ");
        _to_send.append(_date);
        _to_send.append("\r\n");
    }
    if (!_server.empty())
    {
        _to_send.append("Server: ");
        _to_send.append(_server);
        _to_send.append("\r\n");
    }
    if (!_transfer_encoding.empty())
    {
        _to_send.append("Transfer-Encoding: ");
        _to_send.append(_transfer_encoding);
        _to_send.append("\r\n");
    }
    if (!_content_type.empty())
    {
        _to_send.append("Content-Type:");
        i = 0;
        while (i < _content_type.size())
        {
            _to_send.append(" ");
            _to_send.append(_content_type[i]);
            if (i + 1 < _content_type.size())
                _to_send.append(";");
            i++;
        }
        _to_send.append("\r\n");
    }

    // 3. Request body
    _to_send.append("\r\n");
    if (!_body.empty())
    {
        _to_send.append(_body);
        LOG_WRT(Logger::DEBUG, "BODY EMPTY IN RESPONSE");
    }
    return (1);
}

/*
** public class methods
*/

int Response::format_to_send(Request *req)
{
    // Status Line
    _http_version = "HTTP/1.1";
    _reason_phrase = code_to_reason[_status_code];

    _content_length = _body.length(); // https://stackoverflow.com/questions/13821263/should-newline-be-included-in-http-response-content-length

    // Response headers, dans l'ordre du sujet
    _date = get_date();
    _server = g_conf._webserv;
    if ((req->_method == "GET" || req->_method == "HEAD" || req->_method == "PUT" || req->_method == "POST") && (_status_code == OK_200 || _status_code == CREATED_201) && _content_type[0] == "")
        _content_type[0] = get_content_type(req->_file);
    if (_status_code == CREATED_201)
        _location = get_location_header(req);
    _content_language.clear();
    _content_location.clear();
    if (!req->_body_file.empty())
        _transfer_encoding = "chunked";
    else
        _transfer_encoding.clear();
    _www_authenticate.clear();
    // Response body
    if (req->_method == "HEAD")
        _body.clear();
    concat_to_send();
    return (1);
}

void Response::handle_response(Request *req)
{
    if (service_unavailable(req)) // 503
        return;
    else if (bad_request(req)) // 400
        return;
    else if (method_not_allowed(req)) // 405
        return;
    else if (unauthorized(req)) // 401
        return;
    else if (request_entity_too_large(req)) // 413
        return;
    else if (_status_code == NOT_FOUND_404) // 404
        not_found(req);
    else if (req->_method == "GET" || req->_method == "HEAD")
        get(req);
    else if (req->_method == "POST")
        post(req);
    else if (req->_method == "PUT")
        put(req);
    else if (req->_method == "DELETE")
        ft_delete(req);
    else if (req->_method == "OPTIONS")
        option(req);
    else if (req->_method == "TRACE")
        trace(req);
    else if (req->_method == "CONNECT")
        connect(req);
}

int Response::accepted_method(Request *req)
{
    return (req->_method == "GET" || req->_method == "HEAD" || req->_method == "PUT" || req->_method == "POST" || req->_method == "DELETE" || req->_method == "OPTIONS" || req->_method == "TRACE" || req->_method == "CONNECT");
}

int Response::bad_request(Request *req)
{
    if (req->_client->_wfd == -1 && req->_client->_rfd == -1)
    {
        if (accepted_method(req))
            return (0);
        LOG_WRT(Logger::DEBUG, "BAD_REQUEST_400\n");
        _status_code = BAD_REQUEST_400;
        _allow = vector_to_string(req->_location->_method, ',');
        std::string path = std::string(_client->_server->_error + "/400.html");
        req->_client->_rfd = open(path.c_str(), O_RDONLY);
        FD_SET(req->_client->_rfd, &g_conf._save_readfds);
        g_conf.add_fd(req->_client->_rfd);
        return (1);
    }
    else
    {
        if (accepted_method(req))
            return (0);
        FD_CLR(req->_client->_rfd, &g_conf._save_readfds);
        g_conf.remove_fd(req->_client->_rfd);
        close(req->_client->_rfd);
        req->_client->_rfd = -1;
        return (1);
    }
}

int Response::method_not_allowed(Request *req)
{
    LOG_WRT(Logger::DEBUG, "method_not_allowed()");
    if (req->_client->_wfd == -1 && req->_client->_rfd == -1)
    {
        for (std::size_t i = 0; i < (req->_location->_method).size(); ++i)
        {
            LOG_WRT(Logger::DEBUG, "test if " + (req->_location->_method)[i] + " == " + req->_method);
            if ((req->_location->_method)[i] == req->_method)
            {
                LOG_WRT(Logger::DEBUG, "method " + req->_method + " is allowed");
                return (0);
            }
        }
        LOG_WRT(Logger::DEBUG, "METHOD_NOT_ALLOWED_405\n");
        _status_code = METHOD_NOT_ALLOWED_405;
        _allow = vector_to_string(req->_location->_method, ',');
        std::string path = std::string(_client->_server->_error + "/405.html");
        req->_client->_rfd = open(path.c_str(), O_RDONLY);
        FD_SET(req->_client->_rfd, &g_conf._save_readfds);
        g_conf.add_fd(req->_client->_rfd);
        return (1);
    }
    else
    {
        for (std::size_t i = 0; i < (req->_location->_method).size(); ++i)
        {
            LOG_WRT(Logger::DEBUG, "test if " + (req->_location->_method)[i] + " == " + req->_method);
            if ((req->_location->_method)[i] == req->_method)
                return (0);
        }
        FD_CLR(req->_client->_rfd, &g_conf._save_readfds);
        g_conf.remove_fd(req->_client->_rfd);
        close(req->_client->_rfd);
        req->_client->_rfd = -1;
        return (1);
    }
}

// https://stackoverflow.com/questions/180947/base64-decode-snippet-in-c

static const std::string base64_chars =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";

static inline bool is_base64(unsigned char c)
{
    return (isalnum(c) || (c == '+') || (c == '/'));
}

std::string base64_decode(std::string const &encoded_string)
{
    int in_len = encoded_string.size();
    int i = 0;
    int j = 0;
    int in_ = 0;
    unsigned char char_array_4[4], char_array_3[3];
    std::string ret;

    while (in_len-- && (encoded_string[in_] != '=') && is_base64(encoded_string[in_]))
    {
        char_array_4[i++] = encoded_string[in_];
        in_++;
        if (i == 4)
        {
            for (i = 0; i < 4; i++)
                char_array_4[i] = base64_chars.find(char_array_4[i]);

            char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
            char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
            char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

            for (i = 0; (i < 3); i++)
                ret += char_array_3[i];
            i = 0;
        }
    }
    if (i)
    {
        for (j = i; j < 4; j++)
            char_array_4[j] = 0;

        for (j = 0; j < 4; j++)
            char_array_4[j] = base64_chars.find(char_array_4[j]);

        char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
        char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
        char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

        for (j = 0; (j < i - 1); j++)
            ret += char_array_3[j];
    }

    return ret;
}

int Response::unauthorized(Request *req)
{
    if (req->_client->_wfd == -1 && req->_client->_rfd == -1)
    {
        if (!req->_location->_auth.empty())
        {
            LOG_WRT(Logger::DEBUG, "inside unauthorized():");

            std::vector<std::string> tokens;
            tokens = utils_tmp::split(req->_authorization, ' ');
            std::string creds = tokens[1];
            if (req->_location->_auth == base64_decode(creds))
            {
                LOG_WRT(Logger::INFO, "Response::unauthorized() ? -> ok authorized\n");
                _status_code = OK_200;
                return (0);
            }
            else
            {
                LOG_WRT(Logger::INFO, "Response::unauthorized() ? -> ko unauthorized\n");
                _status_code = UNAUTHORIZED_401;
                std::string path = std::string(_client->_server->_error + "/401.html");
                req->_client->_rfd = open(path.c_str(), O_RDONLY);
                FD_SET(req->_client->_rfd, &g_conf._save_readfds);
                g_conf.add_fd(req->_client->_rfd);
                return (1);
            }
        }
        else
            return (0);
    }
    else
    {
        if (!req->_location->_auth.empty())
        {
            std::vector<std::string> tokens;
            tokens = utils_tmp::split(req->_authorization, ' ');
            std::string creds = tokens[1];
            if (req->_location->_auth == base64_decode(creds))
            {
                _status_code = OK_200;
                return (0);
            }
            else
            {
                FD_CLR(req->_client->_rfd, &g_conf._save_readfds);
                g_conf.remove_fd(req->_client->_rfd);
                close(req->_client->_rfd);
                req->_client->_rfd = -1;
                return (1);
            }
        }
        else
            return (0);
    }
}

int Response::service_unavailable(Request *req)
{
    if (req->_client->_wfd == -1 && req->_client->_rfd == -1)
    {
        if (_retry_after == UNAVAILABLE_TIME)
        {
            LOG_WRT(Logger::INFO, "Response::service_unavailable()\n");
            _status_code = SERVICE_UNAVAILABLE_503;
            std::string path = std::string(_client->_server->_error + "/503.html");
            req->_client->_rfd = open(path.c_str(), O_RDONLY);
            FD_SET(req->_client->_rfd, &g_conf._save_readfds);
            g_conf.add_fd(req->_client->_rfd);
            return (1);
        }
        else
            return (0);
    }
    else
    {
        if (_retry_after == UNAVAILABLE_TIME)
        {
            FD_CLR(req->_client->_rfd, &g_conf._save_readfds);
            g_conf.remove_fd(req->_client->_rfd);
            close(req->_client->_rfd);
            req->_client->_rfd = -1;
            return (1);
        }
        else
            return (0);
    }
}

int Response::request_entity_too_large(Request *req)
{
    if (req->_client->_wfd == -1 && req->_client->_rfd == -1)
    {
        if (req->_saved_error == REQUEST_ENTITY_TOO_LARGE_413 || (req->_content_length > 0 && req->_location->_max_body > 0))
        {
            if (req->_saved_error == REQUEST_ENTITY_TOO_LARGE_413 || (req->_content_length > req->_location->_max_body))
            {
                LOG_WRT(Logger::INFO, "Response::request_entity_too_large()\n");
                _status_code = REQUEST_ENTITY_TOO_LARGE_413;
                std::string path = std::string(_client->_server->_error + "/413.html");
                req->_client->_rfd = open(path.c_str(), O_RDONLY);
                LOG_WRT(Logger::DEBUG, "Response::request_entity_too_large() path =" + path + " | _rfd =" + std::to_string(req->_client->_rfd) + "\n");
                FD_SET(req->_client->_rfd, &g_conf._save_readfds);
                g_conf.add_fd(req->_client->_rfd);
                return (1);
            }
        }
    }
    else
    {
        if (req->_saved_error == REQUEST_ENTITY_TOO_LARGE_413 || (req->_content_length > 0 && req->_location->_max_body > 0))
        {
            if (req->_saved_error == REQUEST_ENTITY_TOO_LARGE_413 || (req->_content_length > req->_location->_max_body))
            {
                FD_CLR(req->_client->_rfd, &g_conf._save_readfds);
                g_conf.remove_fd(req->_client->_rfd);
                close(req->_client->_rfd);
                req->_client->_rfd = -1;
                return (1);
            }
        }
    }
    return (0);
}

int Response::not_found(Request *req)
{
    LOG_WRT(Logger::DEBUG, "inside notfound()");
    if (req->_client->_wfd == -1 && req->_client->_rfd == -1)
    {
        _status_code = NOT_FOUND_404;
        std::string path = std::string(_client->_server->_error + "/404.html");
        req->_client->_rfd = open(path.c_str(), O_RDONLY);
        FD_SET(req->_client->_rfd, &g_conf._save_readfds);
        g_conf.add_fd(req->_client->_rfd);
    }
    else
    {
        FD_CLR(req->_client->_rfd, &g_conf._save_readfds);
        g_conf.remove_fd(req->_client->_rfd);
        close(req->_client->_rfd);
        req->_client->_rfd = -1;
    }
    return (1);
}

int Response::build_chunked(Request &req, char *buffer, int ret)
{
    std::string tmp;
    tmp = buffer;
    if (req._is_body_file_header)
    {
        utils_tmp::extract_body(tmp);
        ret = tmp.length();
        req._is_body_file_header = false;
    }
    _to_send.append(utils_tmp::dec_to_hex(ret) + "\r\n");
    _to_send.append(tmp);
    _to_send.append("\r\n");
    LOG_WRT(Logger::DEBUG, "_to_send ==>[" + std::to_string(_to_send.length()) + "]");
    if (ret == 0)
    {
        FD_CLR(_client->_rfd, &g_conf._save_readfds);
		print_all_fd();
        g_conf.remove_fd(_client->_rfd);
		print_all_fd();
        close(read_fd);
        read_fd = -1;
        _client->_rfd = -1;
        _client->_is_finished = true;
        remove(req._body_file.c_str());
    }
    return (0);
}

/*
** Reset before waiting for new call
*/

void Response::reset(void)
{
    init();
}
