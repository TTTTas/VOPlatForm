#include "EpipolarGeometryWorker.h"
#include "EpipolarGeometry.h"

void EpipolarGeometryWorker::process()
{
	EpipolarGeometry::Run(this);
	emit finished();
}

void EpipolarGeometryWorker::process_VO()
{
	EpipolarGeometry::VORun(this);
	emit finished();
}

void EpipolarGeometryWorker::cancel() {
	canceled = true;
}
