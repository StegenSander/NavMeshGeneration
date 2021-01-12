#include "pch.h"
#include "AStar.h"
#include "Graph.h"
#include "Heuristics.h"
#include <iostream>

AStar::AStar(Graph* pGraph)
    :m_pGraph{ pGraph }
{
}

std::vector<Node*> AStar::FindPath(Node* pStartNode, Node* pEndNode)
{
    std::cout << "PATH SEARCH STARTED...\n";
    std::vector<Node*> path{};

    std::vector<NodeRecord> openList;
    std::vector<NodeRecord> closedList;

    openList.push_back(NodeRecord{ pStartNode,nullptr,0,Heuristics::GetDistance(pStartNode,pEndNode) });

    NodeRecord currentRecord{};
    while (!openList.empty())
    {
        std::cout << "\tNew loop started...\n";
        //Find best possible record
        currentRecord = openList.front();
        for (const NodeRecord& nodeRecord : openList)
        {
            if (nodeRecord < currentRecord)
            {
                currentRecord = nodeRecord;
            }
        }

        //Check if current Node is the final Node
        if (currentRecord.pNode == pEndNode)
        {
            std::cout << "GOAL REACHED...\n";
            closedList.push_back(currentRecord);
            break;
        }

        //Loop over connections
        std::cout << "\t\tLooping over connections...\n";
        for (Connection* pConnection : m_pGraph->GetConnectionsOfNode(currentRecord.pNode))
        {
            float totalCurrentCost = currentRecord.currentCost + pConnection->GetCost();

            bool FoundBetterConnectionToNode{ false };

            //Check if there already is a better connection to the adding node in the Cosed List
            //If true, check which has the shorter path and delete the other
            for (size_t i = 0; i < closedList.size(); i++)
            {
                if (closedList[i].pNode == pConnection->GetOther(currentRecord.pNode))
                {
                    if (totalCurrentCost < closedList[i].currentCost)
                    {
                        closedList.erase(closedList.begin() + i);
                    }
                    else
                    {
                        FoundBetterConnectionToNode = true;
                    }
                }
            }

            //Check if there already is a better connection to the adding node in the Open List
            //If true, check which has the shorter path and delete the other
            for (size_t i = 0; i < openList.size(); i++)
            {
                if (openList[i].pNode == pConnection->GetOther(currentRecord.pNode))
                {
                    if (totalCurrentCost < openList[i].currentCost)
                    {
                        openList.erase(openList.begin() + i);
                    }
                    else
                    {
                        FoundBetterConnectionToNode = true;
                    }
                }
            }

            if (!FoundBetterConnectionToNode)
            {
                std::cout << "\t\tAdding new OpenList Record...\n";
                NodeRecord newRecord{};
                newRecord.pNode = pConnection->GetOther(currentRecord.pNode);
                newRecord.pConnection = pConnection;
                newRecord.currentCost = totalCurrentCost;
                newRecord.estimatedCost = totalCurrentCost + Heuristics::GetDistance(newRecord.pNode, pEndNode);
                openList.push_back(newRecord);
            }
        }

        std::cout << "\tEnding loop...\n";
        openList.erase(std::find(openList.begin(), openList.end(), currentRecord));
        closedList.push_back(currentRecord);
    }

    //Make sure a valid path is found
    auto closedIt = std::find_if(closedList.begin(), closedList.end(),
        [pEndNode](const NodeRecord& nodeRecord)
        {
            return nodeRecord.pNode == pEndNode;
        });
    if (closedIt == closedList.end())
    {
        std::cout << "NO PATH WAS FOUND...\n";
        return path;
    }

    //Track back
    std::cout << "\nTRACING BACK PATH...\n";
    Node* previousNode = pEndNode;
    Node* currentNode = pEndNode;

    while (currentNode != pStartNode)
    {
        previousNode = currentNode;
        path.push_back(currentNode);
        std::cout << "\tNode added...\n";

        currentNode = currentRecord.pConnection->GetOther(currentRecord.pNode);

        for (const NodeRecord& nodeRecord : closedList)
        {
            if (nodeRecord.pNode == currentNode)
            {
                currentRecord = nodeRecord;
            }
        }
    }

    path.push_back(pStartNode);
    std::reverse(path.begin(), path.end());

    std::cout << "PATH TRACING COMPLETED...\n";


    return path;
}
