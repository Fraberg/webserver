#ifndef CLIENT_HPP
#define CLIENT_HPP

/*
** Libraries
*/

#include <string>
#include <iostream>

/*
** Headers
*/

/*
** Class
*/

class Client
{
    /*
    ** member variables
    */

    private:
        // 

    protected:
        //

    public:
        int     _accept_fd;

    /*
    ** methods
    */

    private:
        Client();

    protected:
        //

    public:
        Client(int accept_fd);


    /*
    ** friends
    */

    friend class Conf;
    friend class Server;
};

#endif