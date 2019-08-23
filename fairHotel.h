//
// Created by Qi Liu on 2019-04-04.
//

#ifndef PETHOTEL_FAIRHOTEL_H
#define PETHOTEL_FAIRHOTEL_H

#include <iostream>
#include <atomic>
#include <mutex>
#include <condition_variable>

class FairHotel{
private:
    std::atomic<int> totalCats;
    std::atomic<int> totalDogs;
    std::atomic<int> totalBirds;
    std::atomic<int> numOfBirds;
    std::atomic<int> numOfCats;
    std::atomic<int> numOfDogs;
    std::mutex mtx;
    std::condition_variable noCats, noDogsOrBirds;
    void play() const;
    std::atomic<bool>& isDone;
public:
    FairHotel(std::atomic<bool>& done) : isDone(done) {}
    void bird(){
        {
            std::unique_lock<std::mutex> lck(mtx);
            while (numOfCats > 0 || (totalBirds > totalCats && !isDone)) {
                noCats.wait(lck);
            }
            assert(numOfCats == 0);
            numOfBirds++;
            totalBirds++;
        }
        play();
        {
            std::unique_lock<std::mutex> lck(mtx);
            numOfBirds--;
            if(numOfDogs+numOfBirds == 0 || totalCats < totalBirds) noDogsOrBirds.notify_all();

        }
    }
    void cat(){
        {
            std::unique_lock<std::mutex> lck(mtx);
            while (numOfDogs + numOfBirds > 0 || ((totalCats > totalDogs+totalBirds) && !isDone)) {
                noDogsOrBirds.wait(lck);
            }
            assert(numOfDogs + numOfBirds == 0);
            numOfCats++;
            totalCats++;
        }
        play();
        {
            std::unique_lock<std::mutex> lck(mtx);
            numOfCats--;
            if(numOfCats==0 || numOfCats > totalBirds + totalDogs)noCats.notify_all();

        }

    }
    void dog(){
        {
            std::unique_lock<std::mutex> lck(mtx);
            while(numOfCats>0 || ((totalDogs > totalCats) && !isDone)){
                noCats.wait(lck);
            }
            assert(numOfCats == 0);
            numOfDogs++;
            totalDogs++;
        }
        play();
        {
            std::unique_lock<std::mutex> lck(mtx);
            numOfDogs--;
            if(numOfDogs+numOfBirds == 0 || totalCats < totalDogs)  noDogsOrBirds.notify_all();
        }

    }
};

#endif //PETHOTEL_FAIRHOTEL_H
