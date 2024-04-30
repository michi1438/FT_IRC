/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi_handler.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: robin <robin@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/24 16:06:26 by robin             #+#    #+#             */
/*   Updated: 2024/04/30 10:58:42 by mguerga          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../headers/Centralinclude.hpp"

std::string execute_cgi_script(const std::string& cgi_script_path, RequestParser& Req)
{
    int pipefd[2];
    if (pipe(pipefd) == -1)
    {
        perror("Error in pipe");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();
    if (pid == -1)
    {
        perror("Error in fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0)
    {
        // Child process
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);

        // Set environment variables
        setenv("REQUEST_METHOD", Req.getMethod().c_str(), 1);
        setenv("SCRIPT_FILENAME", cgi_script_path.c_str(), 1);
        //setenv("QUERY_STRING", Req.getQueryParam().c_str(), 1);
        std::ostringstream oss;
        oss << Req.getContentLength();
        std::string contentLengthStr = oss.str();
        setenv("CONTENT_LENGTH", contentLengthStr.c_str(), 1);
        setenv("CONTENT_TYPE", Req.getContentType().c_str(), 1);
        //std::cout << "getMethod : " << Req.getMethod().c_str() << "<br>" << std::endl;
        
        // Execute the CGI script
        if(Req.getScriptName().find(".py") != std::string::npos){
            std::string num1;
            std::string num2;
            std::string oprtr;
            std::map<std::string, std::string> query = Req.getQueryParam();
            std::map<std::string, std::string>::iterator it = query.begin();
            for (it = query.begin(); it != query.end(); it++)
            {
                if (it->first == "number1")
                    num1 = it->second;
                else if (it->first == "number2")
                    num2 = it->second;
                else if (it->first == "operator")
                    oprtr = it->second;
                else
                    std::cerr << "Invalid query parameter" << std::endl;
            }
            execl("/usr/bin/python3", "python3", cgi_script_path.c_str(), num1.c_str(), num2.c_str(), oprtr.c_str(), NULL);
        }
        else if(Req.getScriptName().find(".php") != std::string::npos)
            execl("/usr/bin/php", "php", cgi_script_path.c_str(), NULL);
        perror("Error in execl");
        exit(EXIT_FAILURE);
    }
    else
    {
        // Parent process
        close(pipefd[1]);

        // Read the output of the CGI script
        char buffer[BUFFER_SIZE];
        ssize_t bytes_read;
        std::string cgi_output;
        while ((bytes_read = read(pipefd[0], buffer, sizeof(buffer))) > 0)
        {
            cgi_output.append(buffer, bytes_read);
        }

        close(pipefd[0]);
        waitpid(pid, NULL, 0);
        return cgi_output;
    }
}
