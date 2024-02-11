#include <iostream>
#include <string>
#include "../include/utility.h"
#include "../include/Httplib.h"
#include "../include/Json.h"
/*
int main()
{
    bool choice;
    std::cout << "Choose 0 for text file input and 1 for manual input: ";
    std::cin >> choice;
    // Read file name
    if (choice == 0)
    {
        char fileName[100];
        std::cout << "Enter text file name (including .txt/no spaces): ";
        std::cin >> fileName;
        freopen(fileName, "r", stdin);
    }
    // Define the Petri net
    PetriNet petriNet;
    int n = 0;
    // Add initial and final place
    Place initialPlace("i", 0);
    Place finalPlace("o", 0);
    // Get places from user
    if (choice != 0)
        std::cout << "Enter the number of places (min. 2): ";
    std::cin >> n;
    if (n < 2)
    {
        throw std::runtime_error("The number of places must be at least 2");
    }
    int tokens;
    if (choice != 0)
        std::cout << "Enter the number of tokens for the initial place: ";
    std::cin >> tokens;
    initialPlace.tokens = tokens;
    if (choice != 0)
        std::cout << "Enter the number of tokens for the output place: ";
    std::cin >> tokens;
    finalPlace.tokens = tokens;
    petriNet.addPlace(initialPlace);
    for (int i = 0; i < n - 2; i++)
    {
        if (choice != 0)
            std::cout << "Enter the number of token for place p" << (i + 1) << ": ";
        std::cin >> tokens;
        Place place("p" + std::to_string(i + 1), tokens);
        petriNet.addPlace(place);
    }
    petriNet.addPlace(finalPlace);
    // Print the Places in the Petri net
    std::cout << "Places: ";
    for (const auto &place : petriNet.getPlaces())
    {
        std::cout << place.name << " ";
    }
    std::cout << std::endl;
    // Get transitions from user
    if (choice != 0)
        std::cout << "Enter the number of transitions (min. 1): ";
    int m = 0;
    std::cin >> m;
    if (m < 1)
    {
        throw std::runtime_error("The number of transitions must be at least 1");
    }
    for (int i = 0; i < m; i++)
    {
        if (choice != 0)
            std::cout << "Enter the name of transition t" << (i + 1) << ": ";
        std::string name;
        std::getline(std::cin >> std::ws, name);
        Transition transition(name);
        petriNet.addTransition(transition);
    }
    // Print the Transitions in the Petri net
    if (choice != 0)
    {
        std::cout << "Transitions: ";
    }
    for (int i = 0; i < petriNet.getTransitions().size(); i++)
    {
        if (choice != 0)
            std::cout << "{t" << i + 1 << " , " << petriNet.getTransitions()[i].name << "} ";
    }
    if (choice != 0)
        std::cout << std::endl;
    // Add the arcs
    if (choice != 0)
        std::cout << "Enter number of arcs (min. 1): ";
    int k = 0;
    std::cin >> k;
    if (k < 1)
    {
        throw std::runtime_error("The number of arcs must be at least 1");
    }
    for (int i = 0; i < k; i++)
    {
        std::string x, y, placeName, transitionName;
        if (choice != 0)
            std::cout << "Enter the name of the place (i, p1, o, ..) or transition (t1, t2, ..): ";
        std::cin >> x;
        if (choice != 0)
            std::cout << "Enter the name of the place (i, p1, o, ..) or transition (t1, t2, ..): ";
        std::cin >> y;
        bool firstIsPlace = petriNet.getPlaceMap().count(x);
        bool secondIsPlace = petriNet.getPlaceMap().count(y);
        if (firstIsPlace == secondIsPlace)
        {
            throw std::runtime_error(firstIsPlace ? "Both are places" : "Both are transitions");
        }
        else if (firstIsPlace)
        {
            placeName = x;
            transitionName = y;
        }
        else
        {
            placeName = y;
            transitionName = x;
        }
        if (!petriNet.getPlaceMap().count(placeName))
        {
            throw std::runtime_error("The place " + placeName + " does not exist");
        }
        if (!petriNet.getReverseTransitionMap().count(transitionName))
        {
            throw std::runtime_error("The transition " + transitionName + " does not exist");
        }
        Place place = petriNet.getPlaces()[petriNet.getPlaceMap().at(placeName)];
        Transition transition = petriNet.getReverseTransitionMap().at(transitionName);
        if (firstIsPlace)
        {
            petriNet.addArc(place, transition);
        }
        else
        {
            petriNet.addArc(transition, place);
        }
    }
    // Print Conclusion
    std::cout << "Workflow net: " << std::boolalpha << isWorkflowNet(petriNet) << std::endl;
    bool soundness = isSound(petriNet);
    std::cout << "Soundness: " << std::boolalpha << soundness << std::endl;
    // Print the latest fired transitions
    if (soundness)
    {
        std::cout << "The fired transitions (in order): ";
        for (const auto &x : latestFiredTransitions)
        {
            std::cout << "t" + std::to_string(petriNet.getTransitionMap().at(x.name) + 1) << " ";
        }
        std::cout << std::endl;
    }
    else
    {
        writePetriNetToSVG(petriNet, "1st.svg");
    }

    drawReachabilityGraphToSVG(petriNet, "reachability.svg");
    std::cout << "Reachability graph saved as reachability.svg \n";
    std::cout << "Press any key to exit...";
    std::cin.get();
    std::cout << "Thanks for using the program :)" << std::endl;
    return 0;
}*/

using json = nlohmann::json;

PetriNet parseJSON(const std::string& req) {
    json j = json::parse(req);
    PetriNet petriNet;
    for(int i = 0; i < j["places"].size(); i++) {
        std::string marking = j["places"][i];
        std::string name = std::to_string(i+1);
        if(i + 1 == j["inputPlace"])
            name = "i";
        else if(i + 1 == j["outputPlace"])
            name = "o";
        petriNet.addPlace(Place(name, stoi(marking)));
    }
    for(const auto& transition : j["transitions"]) {
        std::string name = transition;
        std::cout << "Transition: " << name << std::endl;
        petriNet.addTransition(Transition(name));
    }
    for(const auto& arc : j["arcs"]) {
        std::string place = arc["place"];
        std::string transition = arc["transition"];
        std::string direction = arc["direction"];
        std::cout << "Arc: " << place << " " << transition << " " << direction << std::endl;
        if(direction == "Place to Transition") {
            petriNet.addArc(petriNet.getPlaces()[stoi(place)-1], petriNet.getTransitions()[petriNet.getTransitionMap().at(transition)]);
        } else {
            petriNet.addArc(petriNet.getTransitions()[petriNet.getTransitionMap().at(transition)], petriNet.getPlaces()[stoi(place)-1]);
        }
    }
    return petriNet;
}

int main() {
    httplib::Server svr;

    svr.Post("/", [](const httplib::Request &req, httplib::Response &res) {
        std::string js = req.body;
        PetriNet petriNet = parseJSON(js);
        std::cout << (petriNet.toString()) << std::endl;
        isWorkflowNet(petriNet);
        bool soundness = isSound(petriNet);
        std::cout << "Soundness: " << std::boolalpha << soundness << std::endl;
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE");
        res.set_header("Access-Control-Allow-Headers", "Content-Type");
        res.set_header("Access-Control-Allow-Credentials", "true");
        json j;
        j["soundness"] = soundness;
        j["workflowNet"] = isWorkflowNet(petriNet);
        res.set_content(j.dump(), "application/json");
    });

    svr.Options("/", [](const httplib::Request&, httplib::Response& res) {
        res.status = 200;
    });

    svr.listen("localhost", 5200);
    return 0;

}