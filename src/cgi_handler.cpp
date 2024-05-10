/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi_handler.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: robin <robin@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/24 16:06:26 by robin             #+#    #+#             */
/*   Updated: 2024/05/10 15:47:40 by robin            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../headers/Centralinclude.hpp"

void handle_alarm(int sig)
{
    (void) sig;
    exit(EXIT_FAILURE);
}

//crée une fonction qui récupere l'env dans une map
std::map<std::string, std::string> getEnv(const RequestParser &Req)
{
    std::map<std::string, std::string> envMap;
    envMap["SERVER_PROTOCOL"] = Req.getVersion();
    envMap["REQUEST_METHOD"] = Req.getMethod();
    envMap["REQUEST_URI"] = Req.getURI();
    envMap["QUERY_STRING"] = Req.getQueryString();
    envMap["SCRIPT_NAME"] = Req.getScriptName();
    envMap["CONTENT_LENGTH"] = Req.getContentLength();
    envMap["CONTENT_TYPE"] = Req.getContentType();
    envMap["HTTP_HOST"] = Req.getHost();
    envMap["BODY"] = Req.getBody();
    envMap["BOUNDARY"] = Req.getBoundary();

    return envMap;
}

//crée une fonction char **map_to_env(std::map<std::string, std::string> envMap)
char **map_to_env(std::map<std::string, std::string> envMap)
{
    char **env = new char *[envMap.size() + 1];
    int i = 0;
    for (std::map<std::string, std::string>::iterator it = envMap.begin(); it != envMap.end(); it++)
    {
        //std::cout << "OOOOOOOOOO" << it->second << std::endl;
        std::string envVar = it->first + "=" + it->second;
        env[i] = new char[envVar.size() + 1];
        strcpy(env[i], envVar.c_str());
        env[i][envVar.size()] = '\0';
        i++;
    }
    env[i] = NULL;
    return env;
}

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

    if (pid == 0) // This is the child process
    {
        close(pipefd[0]); // Close the read end of the pipe in the child
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);

        // recupere l'env dans envTmp
        std::map<std::string, std::string> envMap = getEnv(Req);
        char **envTmp = map_to_env(envMap);

        
        
        // Execute the CGI script
        char *const argv[] = {
                        new char [cgi_script_path.size() + 1],
                        new char [Req.getURI().size() + 1],
                        NULL
                        };
        strcpy(argv[0], cgi_script_path.c_str());
        argv[0][cgi_script_path.size()] = '\0';
        strcpy(argv[1], Req.getURI().c_str());
        argv[1][Req.getURI().size()] = '\0';
        
        signal(SIGALRM, handle_alarm);
        alarm(4);
        
        execve(cgi_script_path.c_str(), argv, envTmp);
        delete[] argv[0];
        delete[] argv[1];
        perror("Error in execve");
        exit(EXIT_FAILURE);
    }
    else // This is the parent process
    {
        close(pipefd[1]); // Close the write end of the pipe in the parent

        // Wait for the child to finish
        int status;
        waitpid(pid, &status, 0);
        std::cout << "WEXITSTATUS(status) :" << WEXITSTATUS(status) << std::endl;
        std::cout << "WIFEXITED(status) :" << WIFEXITED(status) << std::endl;
        std::cout << "WIFSIGNALED(status) :" << WIFSIGNALED(status) << std::endl;
        std::cout << "WTERMSIG(status) :" << WTERMSIG(status) << std::endl;
        alarm(0);
        if(WTERMSIG(status) == 14)
        {
            throw (504);
        }
        else if(WTERMSIG(status) == 11)
        {
            throw (500);
        }
        // Read the output of the CGI script
        char buffer[BUFFER_SIZE];
        ssize_t bytes_read;
        std::string cgi_output;
        while ((bytes_read = read(pipefd[0], buffer, sizeof(buffer))) > 0)
        {
            cgi_output.append(buffer, bytes_read);
        }

        return cgi_output;
    }
}
