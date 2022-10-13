//
// Created by Catherine on 11/10/2022.
//

#include "fmod_studio.hpp"
#include "fmod.hpp"
#include "common.h"
#include <stdio.h>



#ifndef FMODTESTAPP_LOAD_BANKS_H
#define FMODTESTAPP_LOAD_BANKS_H



#endif //FMODTESTAPP_LOAD_BANKS_H

//Helper function that loads a bank in from File

FMOD_RESULT loadBank(FMOD::Studio::System* system, const char* filename, FMOD::Studio::Bank** bank)
{
    return system->loadBankFile(filename, FMOD_STUDIO_LOAD_BANK_NONBLOCKING, bank);
}


//Helper function to return state as a string

const char* getLoadingStateString(FMOD_STUDIO_LOADING_STATE state, FMOD_RESULT loadResult)
{
    switch (state)
    {
        case FMOD_STUDIO_LOADING_STATE_UNLOADING:
            return "unloading ";
        case FMOD_STUDIO_LOADING_STATE_UNLOADED:
            return "unloaded ";
        case FMOD_STUDIO_LOADING_STATE_LOADING:
            return "loading ";
        case FMOD_STUDIO_LOADING_STATE_LOADED:
            return "loaded ";
        case FMOD_STUDIO_LOADING_STATE_ERROR:
            //Show common errors

            if(loadResult == FMOD_ERR_NOTREADY)
            {
                return "error (rdy)";
            }
            else if(loadResult == FMOD_ERR_FILE_BAD)
            {
                return "error (bad)";
            }
            else if(loadResult == FMOD_ERR_FILE_NOTFOUND)
            {
                return "error (mis)";
            }
            else
            {
                return "error  ";

            }
        default:
            return "????";


    }
}


//Helper function to return handle validity as a string
//Just because the bank handle is valid doesn't mean the bank
//load has completed successfully

const char* getHandleStateString(FMOD::Studio::Bank* bank)
{
    if (bank == NULL)
    {
        return "null  ";

    }
    else if(!bank->isValid())
    {
        return "invalid";
    }
    else
    {
        return "valid ";
    }
}



//Callback to free memory-point allocation when it is safe to do so

FMOD_RESULT F_CALLBACK studioCallback(FMOD_STUDIO_SYSTEM *system, FMOD_STUDIO_SYSTEM_CALLBACK_TYPE type, void *commanddata, void *userdata )
{

    if(type == FMOD_STUDIO_SYSTEM_CALLBACK_BANK_UNLOAD)
    {
        // For memory point it is now safe to free our memory
        FMOD::Studio::Bank* bank = (FMOD::Studio::Bank*)commanddata;
        void* memory;
        ERRCHECK(bank->getUserData(&memory));
        if (memory)
        {
            free(memory);
        }
    }

    return FMOD_OK;

}


int FMOD_Main()
{
    void *extraDriverData = 0;
    Common_Init(&extraDriverData);

    FMOD::Studio::System* system;
    ERRCHECK( FMOD::Studio::System::create(&system));
    ERRCHECK( system->initialize(1024, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, extraDriverData));
    ERRCHECK( system->setCallback(studioCallback, FMOD_STUDIO_SYSTEM_CALLBACK_BANK_UNLOAD) );

    static const int BANK_COUNT = 4;
    static const char* BANK_NAMES[] =
            {
                    "DesertSounds.bank",
                    "ForestSounds.bank",
                    "SpaceSounds.bank",
                    "Meditations.bank",


            };

    FMOD::Studio::Bank* banks[BANK_COUNT] = {0};
    bool wantBankLoaded[BANK_COUNT] = {0};
    bool wantSampleLoad = true;

    do{
        Common_Update();

        for(int i=0; i<BANK_COUNT; i++)
        {
            if(Common_BtnPress((Common_Button)(BTN_ACTION1 + i)))
            {

                //Toggle bank load or unload
                if(!wantBankLoaded[i])
                {


                    ERRCHECK(loadBank(system, Common_MediaPath(BANK_NAMES[i]), &banks[i]));
                            wantBankLoaded[i] = true;


                }
                else
                {
                    ERRCHECK(banks[i]->unload());
                    wantBankLoaded[i] = false;

                }

            }
        }

        if (Common_BtnPress( BTN_MORE))
        {
            wantSampleLoad = !wantSampleLoad;

        }

        //Load bank sample data
        FMOD_RESULT loadStateResult[BANK_COUNT] = {FMOD_OK, FMOD_OK, FMOD_OK, FMOD_OK };
        FMOD_RESULT sampleStateResult[BANK_COUNT] = {FMOD_OK, FMOD_OK, FMOD_OK, FMOD_OK};
        FMOD_STUDIO_LOADING_STATE bankLoadState[BANK_COUNT] = {FMOD_STUDIO_LOADING_STATE_UNLOADED, FMOD_STUDIO_LOADING_STATE_UNLOADED, FMOD_STUDIO_LOADING_STATE_UNLOADED, FMOD_STUDIO_LOADING_STATE_UNLOADED  };
        FMOD_STUDIO_LOADING_STATE sampleLoadState[BANK_COUNT] = { FMOD_STUDIO_LOADING_STATE_UNLOADED, FMOD_STUDIO_LOADING_STATE_UNLOADED, FMOD_STUDIO_LOADING_STATE_UNLOADED, FMOD_STUDIO_LOADING_STATE_UNLOADED };
        for (int i=0; i<BANK_COUNT; i++)
        {
            if (banks[i]  && banks[i]->isValid())
            {
                loadStateResult[i] = banks[i]->getLoadingState(&bankLoadState[i]);

            }
            if (bankLoadState[i] == FMOD_STUDIO_LOADING_STATE_LOADED)
            {
                sampleStateResult[i]  = banks[i]->getSampleLoadingState(&sampleLoadState[i]);
                if(wantSampleLoad && sampleLoadState[i] == FMOD_STUDIO_LOADING_STATE_UNLOADED)
                {
                    ERRCHECK( banks[i]->loadSampleData());

                }
                else if(!wantSampleLoad && ( sampleLoadState[i]  == FMOD_STUDIO_LOADING_STATE_LOADING || sampleLoadState[i] == FMOD_STUDIO_LOADING_STATE_LOADED))
                {
                    ERRCHECK(banks[i]->unloadSampleData());
                }
            }
        }


        ERRCHECK( system->update());

        Common_Draw("==================================================");
        Common_Draw("===============Bank load Test ====================");
        Common_Draw("==================================================");

        for( int i = 0; i < BANK_COUNT; ++i)
        {
            char namePad[64] = {0};
            int bankNameLen = strlen(BANK_NAMES[i]);
            memset(namePad, '___', 15);
            strncpy(namePad, BANK_NAMES[i], bankNameLen);

            Common_Draw("%s %s %s %s",
                        namePad,
                        getHandleStateString(banks[i]),
                        getLoadingStateString(bankLoadState[i], loadStateResult[i]),
                        getLoadingStateString(sampleLoadState[i], sampleStateResult[i]));

        }

        Common_Draw("");
        Common_Draw("Press %s to load bank 1", Common_BtnStr(BTN_ACTION1));
        Common_Draw("press %s to load bank 2", Common_BtnStr(BTN_ACTION2));
        Common_Draw("press %s to load bank 3", Common_BtnStr(BTN_ACTION3));
        Common_Draw("press %s to load bank 4", Common_BtnStr(BTN_ACTION4));

        Common_Draw("Press %s to toggle sample data", Common_BtnStr(BTN_MORE));
        Common_Draw("Press %s to quit", Common_BtnStr(BTN_QUIT));

        Common_Sleep(50);




    } while( !Common_BtnPress(BTN_QUIT));

    ERRCHECK(system->unloadAll());
    ERRCHECK(system->flushCommands());
    ERRCHECK(system->release());

    Common_Close();

    return 0;

}



































