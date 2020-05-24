#pragma once

#include "CoreMinimal.h"
#include "EyeTracker/Public/IEyeTracker.h"
#include "Containers/Ticker.h"
#include "GameFramework/PlayerController.h"


#define EyeTrackingMode 6

//USTRUCT(BlueprintType)
struct FPicoEyeTrackingData
{
	//GENERATED_USTRUCT_BODY()

		FPicoEyeTrackingData() :
		leftEyePoseStatus(0),
		rightEyePoseStatus(0),
		combinedEyePoseStatus(0),
		leftEyeGazePoint(FVector::ZeroVector),
		rightEyeGazePoint(FVector::ZeroVector),
		combinedEyeGazePoint(FVector::ZeroVector),
		leftEyeGazeVector(FVector::ZeroVector),
		rightEyeGazeVector(FVector::ZeroVector),
		combinedEyeGazeVector(FVector::ZeroVector),
		leftEyeOpenness(0.f),
		rightEyeOpenness(0.f),
		leftEyePupilDilation(0.f),
		rightEyePupilDilation(0.f),
		leftEyePositionGuide(FVector::ZeroVector),
		rightEyePositionGuide(FVector::ZeroVector)
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PicoEyeTrackingData")
		int32  leftEyePoseStatus;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PicoEyeTrackingData")
		int32  rightEyePoseStatus;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PicoEyeTrackingData")
		int32  combinedEyePoseStatus;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PicoEyeTrackingData")
		FVector  leftEyeGazePoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PicoEyeTrackingData")
		FVector  rightEyeGazePoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PicoEyeTrackingData")
		FVector  combinedEyeGazePoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PicoEyeTrackingData")
		FVector  leftEyeGazeVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PicoEyeTrackingData")
		FVector  rightEyeGazeVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PicoEyeTrackingData")
		FVector  combinedEyeGazeVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PicoEyeTrackingData")
		float  leftEyeOpenness;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PicoEyeTrackingData")
		float	rightEyeOpenness;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PicoEyeTrackingData")
		float  leftEyePupilDilation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PicoEyeTrackingData")
		float   rightEyePupilDilation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PicoEyeTrackingData")
		FVector   leftEyePositionGuide;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PicoEyeTrackingData")
		FVector   rightEyePositionGuide;

};

struct FEyeTrackingGazeRay
{
	FVector Direction;				//Vector in world space with the gaze direction.
	bool IsValid;					//IsValid is true when there is available gaze data.
	FVector Origin;				//The middle of the eyes in world space.
};

enum FEyePoseStatus
{
	kGazePointValid = (1 << 0),
	kGazeVectorValid = (1 << 1),
	kEyeOpennessValid = (1 << 2),
	kEyePupilDilationValid = (1 << 3),
	kEyePositionGuideValid = (1 << 4)
};

class FPicoEyeTracker : public IEyeTracker, public FTickerObjectBase
{

public:
	FPicoEyeTracker();
	virtual ~FPicoEyeTracker();
	virtual bool Tick(float DeltaTime) override;

	static FPicoEyeTracker* GetInstance()
	{
		if (gEyeTracker == nullptr)
		{
			gEyeTracker = new FPicoEyeTracker();
		}
		return gEyeTracker;
	}

	void Destory()
	{
		if (nullptr == gEyeTracker)
		{
			delete gEyeTracker;
			gEyeTracker = nullptr;
		}
	}

	/**
	 * Specifies player being eye-tracked. This is not necessary for all devices, but is necessary for some to determine viewport properties, etc..
	 * Implementing class should cache this locally as it doesn't need to be called every tick.
	 */
	virtual void SetEyeTrackedPlayer(APlayerController* PlayerController) override;

	/**
	 * Returns gaze data for the given player controller.
	 * @return true if returning valid data, false if gaze data was unavailable
	*/
	virtual bool GetEyeTrackerGazeData(FEyeTrackerGazeData& OutGazeData) const override;

	/**
	 * Returns stereo gaze data for the given player controller (contains data for each eye individually).
	 * @return true if returning valid data, false if stereo gaze data was unavailable
	 */
	virtual bool GetEyeTrackerStereoGazeData(FEyeTrackerStereoGazeData& OutGazeData) const override;

	/** Returns information about the status of the current device. */
	virtual EEyeTrackerStatus GetEyeTrackerStatus() const override;


	/** Returns true if the current device can provide per-eye gaze data, false otherwise. */
	virtual bool IsStereoGazeDataAvailable() const override;


	bool OpenEyeTracking();

	bool GetEyeTrackingDataFromDevice(FPicoEyeTrackingData &TrackingData);

	bool GetEyeTrackingGazeRay(FEyeTrackingGazeRay &EyeTrackingGazeRay)const;

	bool GetEyeDirectionToFFR(FVector &OutDirection)const;

	bool isEyeGazeDirectionValid(int32& Status)  const { return  0 != (Status & FEyePoseStatus::kGazeVectorValid); }

	bool isEyeGazePointValid(int32_t& Status)      const { return  0 != (Status & FEyePoseStatus::kGazePointValid); }

	TWeakObjectPtr<APlayerController> ActivePlayerController;
	FPicoEyeTrackingData TrackerData;
	bool bEyeTrackingRun;
	static FPicoEyeTracker* gEyeTracker;

};