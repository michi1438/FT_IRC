/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi_handler.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: robin <robin@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/24 16:06:26 by robin             #+#    #+#             */
/*   Updated: 2024/05/02 16:02:30 by robin            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../headers/Centralinclude.hpp"

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
        strcpy(const_cast<char*>(argv[0]), cgi_script_path.c_str());
        argv[0][cgi_script_path.size()] = '\0';
        strcpy(const_cast<char*>(argv[1]), Req.getURI().c_str());
        argv[1][Req.getURI().size()] = '\0';
        
        execve(cgi_script_path.c_str(), argv, envTmp);

        perror("Error in execve");
        exit(EXIT_FAILURE);
    }
    else // This is the parent process
    {
        close(pipefd[1]); // Close the write end of the pipe in the parent

        // Wait for the child to finish
        int status;
        waitpid(pid, &status, 0);

        // Read the output of the CGI script from the pipe
        char buffer[4096];
        read(pipefd[0], buffer, sizeof(buffer));

        // Convert the output to a string
        std::string output(buffer);

        return output;
    }
}
