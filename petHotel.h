//
// Created by Qi Liu on 2019-04-03.
//

#ifndef PETHOTEL_PETHOTEL_H
#define PETHOTEL_PETHOTEL_H

#include <iostream>
#include <atomic>
#include <mutex>
#include <condition_variable>

/**
 * The scenario of petHotel is that once there is a cat , dogs and birds are not allowed to be here and vise verse.
 * This could cause some unfairness, for example, there could be 2/3 chances that cats cannot enter the hotel if we have
 * same number of cats birds and dogs since birds and dogs can be in hotel together but cats cannot. But if cat enter into
 * the hotel first, all birds and dogs have to wait, so only cat threads run.
 */
class PetHotel{
private:
    std::atomic<int> numOfBirds;
    std::atomic<int> numOfCats;
    std::atomic<int> numOfDogs;
    std::mutex mtx;
    std::condition_variable noCats, noDogsOrBirds;
    void play() const;
public:
    void bird(std::atomic<bool> &done){
        {
            std::unique_lock<std::mutex> lck(mtx);
            while (numOfCats > 0) {
                noCats.wait(lck);
            }
            assert(numOfCats == 0);
            numOfBirds++;
        }
        play();
        {
            std::unique_lock<std::mutex> lck(mtx);
            numOfBirds--;
            if(numOfDogs+numOfBirds == 0) noDogsOrBirds.notify_all();

        }
    }
    void cat(std::atomic<bool> &done){
        {
            std::unique_lock<std::mutex> lck(mtx);
            while (numOfDogs + numOfBirds > 0) {
                noDogsOrBirds.wait(lck);
            }
            assert(numOfDogs + numOfBirds == 0);
            numOfCats++;
        }
        play();
        {
            std::unique_lock<std::mutex> lck(mtx);
            numOfCats--;
            if(numOfCats==0)noCats.notify_all();

        }

    }
    void dog(std::atomic<bool> &done){
        {
            std::unique_lock<std::mutex> lck(mtx);
            while(numOfCats>0){
                noCats.wait(lck);
            }
            assert(numOfCats == 0);
            numOfDogs++;
        }
        play();
        {
            std::unique_lock<std::mutex> lck(mtx);
            numOfDogs--;
            if(numOfDogs+numOfBirds == 0) noDogsOrBirds.notify_all();
        }

    }
};

#endif //PETHOTEL_PETHOTEL_H
