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
#include <limits>


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
   int margin;
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
   Ride* last_ride;
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

                  return R1->margin < R2->margin;
               }
            }

        /* }
      }*/

   }
};

int getNearestVehicule(int last_finish, int startx, int starty, Ride* ride, int tick)
{
   int best_vehicule = -1;
   int best_distance = std::numeric_limits<int>::max();
   int new_distance = -1;
   int distance_to_last_start = ride->last_start - tick;
   int distance_to_earliest_start = ride->earliest_start - tick;

   for (auto vehicule : vehicules) 
   {
      if (!vehicule->status)
      {
         continue;
      }

      new_distance = abs(vehicule->startx - startx) + abs(vehicule->starty - starty);

      if (new_distance == distance_to_earliest_start)
      {
         best_vehicule = vehicule->id - 1;
         return best_vehicule;
      }
      else
      {
         if (new_distance < distance_to_earliest_start)
         {
            return -1;
         }
         else
         {
            if (new_distance < distance_to_last_start)
            {
               if (new_distance < best_distance)
               {
                  best_distance = new_distance;
                  best_vehicule = vehicule->id - 1;
               }
            }
         }
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
            vehicule->last_ride = nullptr;
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
         rideList[currentRide]->margin = rideList[currentRide]->last_finish - rideList[currentRide]->earliest_start;
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
         if (ride->start_tick != -1)
            continue;
         int id_vehicule = getNearestVehicule(ride->last_start, ride->startx, ride->starty, ride, tick);
         if (id_vehicule != -1 )
         {
            vehicules[id_vehicule]->myrides.push_back(ride);
            vehicules[id_vehicule]->status = false;
            vehicules[id_vehicule]->last_ride = ride;

            ride->start_tick = tick;
            ride->end_tick = tick + ride->length;
         }

      }
      tick++;
      for (Vehicule* vehicule : vehicules)
      {
         auto ride = vehicule->last_ride;
         
         if(ride != nullptr && ride->end_tick == tick)
         {
            vehicule->status = true;
            vehicule->startx = ride->endx;
            vehicule->starty = ride->endy;
            sortedRides.erase(ride);
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


