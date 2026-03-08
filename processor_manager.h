#pragma once
// processor_manager.h
class IScene;

void    ProcessorM_Initialize();
void    ProcessorM_Finalize();
void    ProcessorM_Update(IScene* pScene);
void    ProcessorM_Draw(IScene* pScene);
