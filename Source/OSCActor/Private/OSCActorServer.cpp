#include "OSCActorServer.h"

#include "OSCClient.h"

AOSCActorServer::AOSCActorServer()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AOSCActorServer::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	OSCServer = NewObject<UOSCServer>(GetTransientPackage(), FName("OSCActorServer"));
	OSCServer->SetAddress(FString("0.0.0.0"), Port);
	OSCServer->Listen();

	UOSCClient *client = NewObject<UOSCClient>();
	client->Connect();
	
	FOSCMessage msg;
	msg.SetAddress(FOSCAddress(FString("/play")));
	
	for (auto x : ControllerIPs)
	{
		client->SetSendIPAddress(x, ControllerPort);
		client->SendOSCMessage(msg);
	}

	client->ConditionalBeginDestroy();
}

void AOSCActorServer::BeginPlay()
{
	Super::BeginPlay();
}

void AOSCActorServer::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	OSCServer->Stop();
}
