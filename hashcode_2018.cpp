// hashcode_2018.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <list>
#include <string>
#include <vector>
#include <unordered_map>
#include <map>
#include <set>
#include <utility>
#include <algorithm>


struct Ride
{
   int id;
   int startx;
   int starty;
   int endx;
   int endy;
   int earliest_start;
   int last_finish;
   int last_start;
   int length;
   int start_tick;
   int end_tick;
   int x;
   int y;
   bool used;
};

struct Vehicule
{
   int id;
   int startx;
   int starty;
   int endx;
   int endy;
   std::vector<Ride*> myrides;
   bool status;
   int ride_id;
   Ride* ride;
};

std::vector<Vehicule*> vehicules;






struct  Comp
{
   bool operator()(Ride* R1, Ride* R2) const
   {
      /*if (R1->last_finish < R2->last_finish)
      {
         return true;
      }
      else
      {
         if (R1->last_finish > R2->last_finish)
         {
            return false;
         }
         else
         {*/
            if (R1->last_start < R2->last_start)
            {
               return true;
            }
            else
            {
               if (R1->last_start > R2->last_start)
                  return false;
               else
               {

                  return R1->earliest_start < R2->earliest_start;
               }
            }

         /*}
      }*/

   }
};

int getNearestVehicule(int last_finish, int startx, int starty, bool& found)
{
   int best_vehicule = 0;
   int best_distance = -1;
   int new_distance = -1;
   found = false;

   for (auto vehicule : vehicules) 
   {

      if (vehicule->startx == startx && vehicule->starty == starty)
      {
         best_vehicule = vehicule->id - 1;
         found = true;
         return best_vehicule;
      }

      if (!vehicule->status)
      {
            continue;
      }
 
      new_distance = abs(vehicule->startx - startx) + abs(vehicule->starty - starty);

      if (best_distance == -1)
      {
         best_distance = new_distance;
         best_vehicule = vehicule->id - 1;
         found = true;

      }    
      else if (new_distance < best_distance) {
        
         best_distance = new_distance;
         best_vehicule = vehicule->id - 1;
         found = true;
      }
      
   }

   return best_vehicule;

}


enum class ReadingState : char
{
   Start,
   RideInput,
   EndReading
};

void split(const std::string& str, const std::string& delimiter, std::vector<std::string>& results)
{
   std::string::size_type pos = 0, cur = 0;

   while ((pos = str.find(delimiter.c_str(), cur)) != std::string::npos)
   {
      std::string word = str.substr(cur, pos - cur);
      results.emplace_back(word);
      cur = pos + 1;
   }

   std::string word = str.substr(cur, str.length() - cur);
   results.emplace_back(word);
}

int main()
{
   std::string line = "";
   ReadingState state = ReadingState::Start;
   int nbRows = 0, nbColumns = 0, nbVehicules = 0, nbRides = 0, nbBonus = 0, nbSteps = 0;
   std::vector<Ride*> rideList;
   std::set<Ride*, Comp> sortedRides;
   int currentRide = 0;


   // Reading input file
   while (std::getline(std::cin, line))
   {
      std::vector<std::string> words;
      split(line, " ", words);

      switch (state)
      {
      case ReadingState::Start:
      {
         nbRows = atoi(words[0].c_str());
         nbColumns = atoi(words[1].c_str());
         nbVehicules = atoi(words[2].c_str());
         nbRides = atoi(words[3].c_str());
         nbBonus = atoi(words[4].c_str());
         nbSteps = atoi(words[5].c_str());

         for (int i = 0; i < nbRides; ++i)
         {
            auto ride = new Ride();
            ride->id = i;
            rideList.push_back(ride);
         }

         for (int i = 0; i < nbVehicules; ++i)
         {
            auto vehicule = new Vehicule();
            vehicule->id = i + 1;
            vehicule->status = true;
            vehicules.push_back(vehicule);
         }

         state = ReadingState::RideInput;
         break;
      }
      case ReadingState::RideInput:
      {
         int sx = atoi(words[0].c_str());
         int sy = atoi(words[1].c_str());
         int ex = atoi(words[2].c_str());
         int ey = atoi(words[3].c_str());
         rideList[currentRide]->startx = sx;
         rideList[currentRide]->starty = sy;
         rideList[currentRide]->endx = ex;
         rideList[currentRide]->endy = ey;
         rideList[currentRide]->length = abs(ey - sy) + abs(ex - sx);
         rideList[currentRide]->earliest_start = atoi(words[4].c_str());
         rideList[currentRide]->last_finish = atoi(words[5].c_str());
         rideList[currentRide]->last_start = rideList[currentRide]->last_finish - rideList[currentRide]->length;
         rideList[currentRide]->id = currentRide;
         rideList[currentRide]->start_tick = -1;
         currentRide++;

         if (currentRide == nbRides)
         {
            state = ReadingState::EndReading;
         }
         break;
      }
      case ReadingState::EndReading:
      {
         break;
      }
      default:
      {
         break;
      }
      }
   }

   // sorting rides
   for (auto ride : rideList)
   {
      sortedRides.insert(ride);
   }


   int tick = 0;
   while (tick < nbSteps)
   {

      for (auto ride : sortedRides)
      {
         bool found = false;
         if (ride->start_tick != -1)
            continue;
         int id_vehicule = getNearestVehicule(ride->last_start, ride->startx, ride->starty, found);
         if (found)
         {
            vehicules[id_vehicule]->myrides.push_back(ride);
            vehicules[id_vehicule]->status = false;
           

            ride->start_tick = tick;

         }
         else
         {

         }

      }
      tick++;
      for (Vehicule* vehicule : vehicules)
      {
         for (auto ride: vehicule->myrides)
         {
            /* need a heuristic to determine, which coordinate (x or y) should be take first ( the best move
            depending of the potential rides that can be take on the path)
            */
            if (vehicule->startx < ride->endx)
               vehicule->startx +=1;
            else
               if (vehicule->startx > ride->endx)
               {
                  vehicule->startx -= 1;
               }
               else
               {
                  if (vehicule->starty < ride->endy)
                     vehicule->starty += 1;
                  else
                     if (vehicule->starty > ride->endy)
                     {
                        vehicule->starty -= 1;
                     }
                     else
                     {
                        vehicule->status = true;

                        sortedRides.erase(ride);
                     }
               }

            }

         }
         
      }

      //Writing output results
      for (auto vehicule : vehicules)
      {
         std::cout << vehicule->myrides.size();
         for (auto ride : vehicule->myrides)
         {
            std::cout << " ";
            std::cout << ride->id;
         }
         std::cout << "\n";
      }

      return 0;
   }


