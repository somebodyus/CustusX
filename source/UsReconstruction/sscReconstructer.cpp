/*
 *  sscReconstructer.cpp
 *  Created by Ole Vegard Solberg on 5/4/10.
 */


#include "sscReconstructer.h"

#include <QtCore>
#include <vtkImageData.h>
#include <vtkUnsignedCharArray.h>
#include <vtkPointData.h>

#include "sscBoundingBox3D.h"
#include "sscDataManagerImpl.h"
#include "sscTypeConversions.h"

typedef vtkSmartPointer<class vtkUnsignedCharArray> vtkUnsignedCharArrayPtr;

namespace ssc
{
Reconstructer::Reconstructer() :
  mAlgorithm(new ThunderVNNReconstructAlgorithm),
  mMaxVolumeSize(256*256*256)
{}
  
QString Reconstructer::changeExtension(QString name, QString ext)
{
  QStringList splitName = name.split(".");
  splitName[splitName.size()-1] = ext;
  return splitName.join(".");
}
  
void Reconstructer::readFiles(QString mhdFileName)
{
  readUsDataFile(mhdFileName);
  
  QString posName = changeExtension(mhdFileName, "pos");
  readPositionFile(posName);
}

void Reconstructer::readUsDataFile(QString mhdFileName)
{
  //Read US images
  mUsRaw = MetaImageReader().load(string_cast(mhdFileName), 
                                  string_cast(mhdFileName));
  
  //Test
  
  /*unsigned char* dataPtr = static_cast<unsigned char*>(mUsRaw->getBaseVtkImageData()->GetScalarPointer());
  int* dim = mUsRaw->getBaseVtkImageData()->GetDimensions();
  //std::cout << "dim: " << ssc::Vector3D(dim) << std::endl;
  //std::cout << "number of points:" 
      for(int z = 0; z < dim[2]; z++)
      {
        //dataPtr[x + y*dim[0] + z*dim[0]*dim[1]] = z/((double)dim[2])*255;
        unsigned char* a = dataPtr + z*dim[0]*dim[1];
        unsigned char* b = dataPtr + (z+1)*dim[0]*dim[1];
        unsigned const char val = z/((double)dim[2])*255;
        std::fill(a, b, val);
      }*/
  
  //Read US image time tags
  QString timeName = changeExtension(mhdFileName, "tim");
  QFile timeFile(timeName);
  if(!timeFile.open(QIODevice::ReadOnly))
  {
    std::cout << "Can't open file: " << string_cast(timeName) << std::endl;
    return;
  }
  while (!timeFile.atEnd())
  {
    bool ok = true;
    UsFrame frame;
    QByteArray array = timeFile.readLine();
    frame.mTime = QString(array).toDouble(&ok);
    if (!ok)
    {
      std::cout << "Can't read double in file: " << string_cast(timeName) << std::endl;
      return;
    }
    mFrames.push_back(frame);
  }
  if(mUsRaw->getBaseVtkImageData()->GetDimensions()[2] != (int)mFrames.size())
  {
    mFrames.clear();
    mUsRaw.reset();
    std::cout << "Number of frames don't match number of positions" << std::endl;
    return;
  }
  std::cout << "Reconstructer::readUsDataFile() - succes. Number of frames: " 
    << mFrames.size() << std::endl;
  return;
}
void Reconstructer::readPositionFile(QString posFile)
{
  QFile file(posFile);
  if(!file.open(QIODevice::ReadOnly))
  {
    std::cout << "Can't open file: " << string_cast(posFile) << std::endl;
    return;
  }
  while (!file.atEnd())
  {
    bool ok = true;
    PositionData position;
    QByteArray array = file.readLine();
    position.mTime = QString(array).toDouble(&ok);
    if (!ok)
    {
      std::cout << "Can't read double in file: " << string_cast(posFile) 
        << std::endl;
      return;
    }
    
    QString positionString = file.readLine();
    positionString += " " + file.readLine();
    positionString += " " + file.readLine();
    positionString += " 0 0 0 1";
    position.mPos = Transform3D::fromString(positionString, &ok);
    if (!ok)
    { 
      std::cout << "Can't read position number: " << mPositions.size() 
        << " from file: " << string_cast(posFile) << std::endl;
      std::cout << "values: " << position.mPos[0][0] << std::endl;;
      return;
    }
    mPositions.push_back(position);
  }
  
  std::cout << "Reconstructer::readPositionFile() - succes. Number of positions: " 
  << mPositions.size() << std::endl;
  return;
}
  
vtkImageDataPtr Reconstructer::generateVtkImageData(Vector3D dim, 
                                                  Vector3D spacing,
                                                  const unsigned char initValue)
{
  vtkImageDataPtr data = vtkImageDataPtr::New();
  data->SetSpacing(spacing[0], spacing[1], spacing[2]);
  data->SetExtent(0, dim[0]-1, 0, dim[1]-1, 0, dim[2]-1);
  data->SetScalarTypeToUnsignedChar();
  data->SetNumberOfScalarComponents(1);
  
  int scalarSize = dim[0]*dim[1]*dim[2];
  
	unsigned char *rawchars = (unsigned char*)malloc(scalarSize+1);
  std::fill(rawchars,rawchars+scalarSize, initValue);
  
  vtkUnsignedCharArrayPtr array = vtkUnsignedCharArrayPtr::New();
  array->SetNumberOfComponents(1);
  //TODO: Whithout the +1 the volume is black 
  array->SetArray(rawchars, scalarSize+1, 0); // take ownership
  data->GetPointData()->SetScalars(array);
  
  rawchars[0] = 255;// A trick to get a full LUT in ssc::Image (automatic LUT generation)
  
  /*data->AllocateScalars();
  unsigned char* dataPtr = static_cast<unsigned char*>(data->GetScalarPointer());
  unsigned long N = data->GetNumberOfPoints();
  N--;//Don't understand this
  for (unsigned long i = 0; i < N; i++)
    dataPtr[i] = 100;
  
  //dataPtr[N] = 255;//This creates a black volume
  
  dataPtr[0] = 255;
  
  std::cout << "Reconstructer::generateOutputVolume() data->GetNumberOfPoints(): " 
  << N << std::endl;*/
  
  
  return data;
}

ImagePtr Reconstructer::generateMask()
{  
  ssc::Vector3D dim(mUsRaw->getBaseVtkImageData()->GetDimensions());
  dim[2] = 1;
  ssc::Vector3D spacing(mUsRaw->getBaseVtkImageData()->GetSpacing());
  
  vtkImageDataPtr data = generateVtkImageData(dim, spacing, 255);
    
  ImagePtr image = ImagePtr(new Image("mask", data, "mask")) ;
  return image;
}
  
ImagePtr Reconstructer::readMaskFile(QString mhdFileName)
{  
  ssc::ImagePtr retval = this->generateMask();
  vtkImageDataPtr data = retval->getBaseVtkImageData();
    
  QString fileName = changeExtension(mhdFileName, "msk");
    
  QFile file(fileName);
  file.open(QIODevice::ReadOnly);
  QDataStream stream(&file);    
        
  unsigned char* dataPtr = static_cast<unsigned char*>(data->GetScalarPointer());  
  char *rawchars = reinterpret_cast<char*>(dataPtr);
  
  stream.readRawData(rawchars, file.size());
  
  return retval;
}
  
/**
 * Apply time calibration function y = ax + b, where
 *  y = calibrated(new) position timestamp
 *  x = old position timestamp
 *  a = input scale
 *  b = input offset
 * \param offset Offset between images and positions. 
 * \param scale Scale between images and positions.
 */
void Reconstructer::calibrateTimeStamps(double offset, double scale)
{
  for (unsigned int i = 0; i < mPositions.size(); i++)
  {
    mPositions[i].mTime = scale * mPositions[i].mTime + offset;
    //std::cout << "postime: " << mPositions[i].mTime << std::endl;
  }
}

/**
 * Calculate timestamp calibration in an adhoc way, by assuming that 
 * images and positions start and stop at the exact same time
 */
void Reconstructer::calibrateTimeStamps()
{
  double scale = (mFrames.back().mTime - mFrames.front().mTime)
    / (mPositions.back().mTime - mPositions.front().mTime);
  
  double offset = mFrames.front().mTime - scale * mPositions.front().mTime;
  
  std::cout << "Reconstructor::calibrateTimeStamps() NB!!! generated offset: " 
    << offset << " scale: " << scale << std::endl;
  calibrateTimeStamps(offset, scale);
}

/**
 * Linear interpolation between a and b. t = 1 means use only b;
 */
Transform3D Reconstructer::interpolate(const Transform3D& a, 
                                       const Transform3D& b,
                                       double t)
{
  Transform3D c;
  for (int i = 0; i < 4; i++)
    for (int j= 0; j < 4; j++)
      c[i][j] = (1-t)*a[i][j] + t*b[i][j];
  return c;
}
  
/**
 * Find interpolated position values for each frame based on the input position
 * data.
 * Current implementation: 
 * Linear interpolation
 */
void Reconstructer::interpolatePositions()
{
  //TODO: Check if the affine transforms still are affine after the linear interpolation 
  
  double scale = mPositions.size() / (double)mFrames.size();
  for(unsigned int i_frame = 0; i_frame < mFrames.size(); i_frame++)
  {
    unsigned int i_pos = i_frame*scale;// =floor()
    if (i_pos < 0)
      i_pos = 0;
    else if(i_pos >= mPositions.size()-1)
      i_pos = mPositions.size()-2;
      
    double t = (mFrames[i_frame].mTime - mPositions[i_pos].mTime) 
      / (mPositions[i_pos+1].mTime - mPositions[i_pos].mTime);
    mFrames[i_frame].mPos = interpolate(mPositions[i_pos].mPos, 
                                        mPositions[i_pos+1].mPos, t);
  }
}
  
/**
 * Reads a whitespace separated 4x4 matrix from file
 * \param fileName Input file
 * \return  The matrix
 */
Transform3D Reconstructer::readTransformFromFile(QString fileName)
{
  Transform3D retval;
  QFile file(fileName);
  if(!file.open(QIODevice::ReadOnly))
  {
    std::cout << "Can't open file: " << string_cast(fileName) << std::endl;
    return retval;
  }
  bool ok = true;
  QString positionString = file.readLine();
  positionString += " " + file.readLine();
  positionString += " " + file.readLine();
  positionString += " " + file.readLine();
  retval = Transform3D::fromString(positionString, &ok);
  if (!ok)
  { 
    std::cout << "Can't read calibration from file: " << string_cast(fileName) << std::endl;
    std::cout << "values: " << retval[0][0] << std::endl;;
    return retval;
  }
  return retval;
}
  
void Reconstructer::applyCalibration(const Transform3D& calibration)
{
  std::cout << " 1st input pos: " << mPositions.front().mPos.coord(ssc::Vector3D(0,0,0)) << std::endl;
  std::cout << "last input pos: " << mPositions.back().mPos.coord(ssc::Vector3D(0,0,0)) << std::endl;
  
  for (unsigned int i = 0; i < mPositions.size(); i++)
    mPositions[i].mPos = calibration * mPositions[i].mPos;
}

void Reconstructer::calibrate(QString calFile)
{
  Transform3D C = this->readTransformFromFile(calFile);
  
  Transform3D Ry = ssc::createTransformRotateY(M_PI/2.0);
  Transform3D Rx = ssc::createTransformRotateX(M_PI/2.0);
  
  Transform3D usMt = Rx*Ry*C;
  applyCalibration(usMt);
  
  
  //TODO: Create offset between input picture origo and US origo
}
  
/**
 * Generate a rectangle (2D) defining ROI in input image space
 */
std::vector<ssc::Vector3D> Reconstructer::generateInputRectangle()
{
  std::vector<ssc::Vector3D> retval(4);
  if(!mMask)
  {
    std::cout << "ERROR Reconstructer::generateInputRectangle() requires mask" << std::endl;
    return retval;
  }
  int* dims = mUsRaw->getBaseVtkImageData()->GetDimensions();
  double* spacing = mUsRaw->getBaseVtkImageData()->GetSpacing();
  
  int xmin = dims[0];
  int xmax = 0;
  int ymin = dims[1];
  int ymax = 0;
  unsigned char* ptr = static_cast<unsigned char*>(mMask->getBaseVtkImageData()->GetScalarPointer());
  for (int x = 0; x < dims[0]; x++)
    for(int y = 0; y < dims[1]; y++)
    {
      unsigned char val = ptr[x + y*dims[0]];
      if (val != 0)
      {
        xmin = std::min(xmin, x);
        ymin = std::min(ymin, y);
        xmax = std::max(xmax, x);
        ymax = std::max(ymax, y);
      }
    }
  
  retval[0] = ssc::Vector3D(xmin*spacing[0], ymin*spacing[1], 0);
  retval[1] = ssc::Vector3D(xmax*spacing[0], ymin*spacing[1], 0);
  retval[2] = ssc::Vector3D(xmin*spacing[0], ymax*spacing[1], 0);
  retval[3] = ssc::Vector3D(xmax*spacing[0], ymax*spacing[1], 0);
  
  std::cout << "x and y, min and max: " << xmin << " " << xmax << " " << ymin << " " << ymax << std::endl;
  
  /*retval[0] = ssc::Vector3D(0,0,0);
  retval[1] = ssc::Vector3D(dims[0]*spacing[0],0,0);
  retval[2] = ssc::Vector3D(0,dims[1]*spacing[1],0);
  retval[3] = ssc::Vector3D(dims[0]*spacing[0],dims[1]*spacing[1],0);*/
  return retval;
}
  
/**
 * Pre:  mFrames[i].mPos = usMpr
 * Post: mFrames[i].mPos = usMd
 *       Output volume is initialized
 */
ImagePtr Reconstructer::generateOutputVolume()
{
  // A first guess for usMd with correct orientation
  Transform3D prMd;
  prMd = mFrames[mFrames.size()/2].mPos;
  prMd = prMd.inv();
  
  for (unsigned int i = 0; i < mFrames.size(); i++)
  {
    mFrames[i].mPos = mFrames[i].mPos * prMd;
  }
  
  // Find extent of all frames as a point cloud
  std::vector<ssc::Vector3D> inputRect = this->generateInputRectangle();
  std::vector<ssc::Vector3D> outputRect;
  for(unsigned int slice = 0; slice < mFrames.size(); slice++)
  {
    Transform3D dMus = mFrames[slice].mPos.inv();
    for (unsigned int i = 0; i < inputRect.size(); i++)
    {
      outputRect.push_back(dMus.coord(inputRect[i]));
    }
  }
  
  /*std::cout << "1st dMus:  " << mFrames.front().mPos.inv().coord(ssc::Vector3D(0,0,0));  
  std::cout << std::endl;
  std::cout << "last dMus: " << mFrames.back().mPos.inv().coord(ssc::Vector3D(0,0,0)); 
  std::cout << std::endl;*/
    
  ssc::DoubleBoundingBox3D boundingBox = ssc::DoubleBoundingBox3D::fromCloud(outputRect);
  ssc::Vector3D bbSize = boundingBox.range();
    
  // Translate usMd to output volume origo
  ssc::Transform3D T_origo = ssc::createTransformTranslate(boundingBox.corner(0,0,0));
  for (unsigned int i = 0; i < mFrames.size(); i++)
  {
    mFrames[i].mPos = mFrames[i].mPos * T_origo;
  }
  
  // Calculate optimal output image spacing and dimensions based on US frame spacing
  double inputSpacing = std::min(mUsRaw->getBaseVtkImageData()->GetSpacing()[0],
                                 mUsRaw->getBaseVtkImageData()->GetSpacing()[1]);
  
  unsigned int xSize = bbSize[0] / inputSpacing;//floor
  unsigned int ySize = bbSize[1] / inputSpacing;
  unsigned int zSize = bbSize[2] / inputSpacing;
  std::cout << "Optimal inputSpacing: " << inputSpacing << std::endl;
  std::cout << "Optimal size: " << xSize << " " << ySize << " " << zSize << std::endl;
  std::cout << "bbSize: " << bbSize << std::endl;
  
  // Reduce output volume size if optimal volume size is too large
  unsigned long volumeSize = xSize*ySize*zSize;
  if (volumeSize > mMaxVolumeSize)
  {
    double scaleFactor = pow(volumeSize/double(mMaxVolumeSize),1/3.0);
    std::cout << "Downsampled volume - Used scaleFactor : " << scaleFactor << std::endl;
    xSize /= scaleFactor;
    ySize /= scaleFactor;
    zSize /= scaleFactor;
    inputSpacing *= scaleFactor;
  }
  
  ssc::Vector3D dim(xSize, ySize, zSize);
  ssc::Vector3D spacing(inputSpacing, inputSpacing, inputSpacing);
  
  std::cout << "output dim: " << dim << std::endl;
  std::cout << "output spacing: " << spacing << std::endl;  
  
  vtkImageDataPtr data = this->generateVtkImageData(dim, spacing, 0);
  
  // Generate volume name and uid
  QString volumeName = qstring_cast(mUsRaw->getName());
  volumeName += "_reconstruct";
  QString volumeId = qstring_cast(mUsRaw->getUid());
  volumeId += "_reconstruct";
  ImagePtr image = ImagePtr(new Image(string_cast(volumeId), 
                                      data, 
                                      string_cast(volumeName))) ;
  return image;
}
  
  
ImagePtr Reconstructer::reconstruct(QString mhdFileName, QString calFileName)
{
  this->readFiles(mhdFileName); 
  //mPos is now tMpr
  
  mMask = this->readMaskFile(mhdFileName);
  
  this->calibrateTimeStamps();
  
  this->calibrate(calFileName);
  //mPos (in mPositions) is now usMpr
  
  this->interpolatePositions();
  // mFrames: now mPos as usMpr
  
  mOutput = this->generateOutputVolume();
  //mPos in mFrames is now usMd
  
  for (unsigned int i; i < mFrames.size(); i++)
    mFrames[i].mPos = mFrames[i].mPos.inv();
  
  QDateTime pre = QDateTime::currentDateTime();
  mAlgorithm->reconstruct(mFrames, mUsRaw, mOutput, mMask);
  std::cout << "Reconstruct time: " << pre.time().msecsTo(QDateTime::currentDateTime().time()) << std::endl;
  
  return mOutput;
}
 
  
ImagePtr Reconstructer::getOutput()
{
  return mOutput;
}
  
}
