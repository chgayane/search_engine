#include <iostream>
#include <stdexcept>
#include <regex.h>
#include <string>
#include <unistd.h>
#include <algorithm>
#include <iterator>
#include "easyhttpcpp/EasyHttp.h"
#include "linkQueue.h"
#include "repo_in_memory.hpp"
#include "indexer.hpp"
#include "DbManager.hpp"

using namespace std;



std::string GetContents(std::string url)
{
	try
	{
		// configure http cache and connection pool instance (optional but recommended)
		easyhttpcpp::EasyHttp::Builder httpClientBuilder;
		// create http client
		easyhttpcpp::EasyHttp::Ptr pHttpClient = httpClientBuilder.build();
		// create a new request and execute synchronously
		easyhttpcpp::Request::Builder requestBuilder;
		easyhttpcpp::Request::Ptr pRequest = requestBuilder.setUrl(url).build();
		easyhttpcpp::Call::Ptr pCall = pHttpClient->newCall(pRequest);
		easyhttpcpp::Response::Ptr pResponse = pCall->execute();

		if (!pResponse->isSuccessful())
		{
			//std::cerr << "HTTP GET Error: (" << pResponse->getCode() << ")" << "-" <<url <<std::endl;
			
			return "";
		}
		else
		{
			std::cout << "HTTP GET Success!" << "-" << url<<std::endl;
			const std::string contentType = pResponse->getHeaderValue("Content-Type", "");
			if (Poco::isubstr<std::string>(contentType, "text/html") != std::string::npos)
			{
				return pResponse->getBody()->toString();
			}
			return "";
		}
	}
	//return url;
	catch (...)
	{
	//	cerr << "Error while getting contents " << endl;
		return "";
	}
}




const int MAX_MATCHES = 10;
linkQueue Linkqueue;
Repo* repo;
Indexer indexer;
//DbManager* dbManager;


void linkAdded(std::string link)
{
	static int count = 0;
	//std::cout << "Crawler:: Link added!! " << link << std::endl;
	if (!(link.find("http")) == 0)
	{
		return;
	}
	//Saves link
	 repo->SaveLink(link); //by me 
	std::string contents = GetContents(link);

	if (contents == "" || contents.size() < 100)
	{
		return;
	}
	//Saves site
	/*dbManager->SaveSite(link, contents);by me */

	 indexer.AddSite(link, contents); 

	const std::string link_prefix = "href=";
	size_t pos = 0;

	while ((pos < contents.size()) && (pos = contents.find(link_prefix, pos + 1)) != std::string::npos)
	{
		pos += link_prefix.size() + 1;
		char quot = contents[pos - 1];
		if (quot != '\'' && quot != '\"')
		{
			continue;
		}

		size_t end_pos = contents.find(quot, pos);

		if (end_pos == std::string::npos)
		{
			break;
		}
		std::string link = contents.substr(pos, end_pos - pos );
		if (!(link.find("http") == 0))
		{
		
			continue;
		}
		
		
		Linkqueue.addLink(link);
	}
}

int run_server();


string findLinkUrl(string text,string &url)
{
	std::string contents = GetContents("https://www.topuniversities.com/");
	

	size_t startLinkPos = text.find("<a");
	if (startLinkPos == -1) {
		cout<< "";
	}
	 contents = text.substr(startLinkPos);

	size_t endLinkPos = contents.find(">");
	if (endLinkPos == -1) {
		cout<<"";
	}
	contents = contents.substr(0, endLinkPos);

	size_t hrefPos = contents.find("href=");
	if (hrefPos == -1) {
		cout<<"";
	}
	string closingScope = contents.substr(hrefPos + 5, 1);
	contents = contents.substr(hrefPos + 6);
	url = contents.substr(0, contents.find(closingScope));
	const int MAX_MATCHES = 10;
	regmatch_t matches[MAX_MATCHES];
for (int i = 0, offset = 0; i < MAX_MATCHES && offset < contents.size(); ++i)
	{
		std::cout << "Matches" << i << "->" << contents.substr(matches[i].rm_so, matches[i].rm_eo - matches[i].rm_so + 1) << std::endl;
		offset = matches[i].rm_eo + 1;
	}

	 
	return text.substr(startLinkPos + 2);
}

int main()
{
	
	std::cout << "Running crawler " << std::endl;
	

	repo = new RepoInMemory();
	//dbManager = new DbManager();
	Linkqueue.registerHandler(linkAdded);
	linkAdded("https://www.topuniversities.com/");
	string url;
	

	
	cout << "Running Server : " << endl;
	run_server();
	cout<<endl << "Running crawler - Done! " << endl;


	//Linkqueue.registerHandler(repo->SaveLink);
	return 0;
}