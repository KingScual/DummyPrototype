/*
* 
* GetStatusWorker.h
* --------------------------------------------------------------
* Description:
* - Retreives the simulated status of the system.
* - Initializes all publishers needed (only 1).
* --------------------------------------------------------------
* 
*/

#pragma once

#include "DummyBitService_Publisher.h"

class GetStatusWorker
{
public:
    // Constructor - Constructs GetStatusWorker class and initalizes publisher
    GetStatusWorker();
    // Destructor
    ~GetStatusWorker();
    // Initiates simulated IBIT. Returns true is successful, false if unsuccessful
    bool StatusRequested(double startTime);
    // Sets the status variable and publishes BIT result.
    void SetStatus(bool bitStatus);
    // Returns status variable member. True == Good, False == Bad
    bool GetStatus();

private:
    //flag to check if publisher was created
	bool publisherCreated = 0; // 0 == not created; 1 == created
    // variable member that holds the most recent status
    bool status = 0; // 0 == bad; 1 == good
    // Publisher class variable
    DummyBITService_Publisher publisher;

    // Simulate BIT API calls and publishes results. Returns true if successful, false if unsuccessful
    bool RunBIT(double startTime);
};