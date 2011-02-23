/*
 * cxPatientData.cpp
 *
 *  Created on: May 18, 2010
 *      Author: christiana
 */
#include "cxPatientData.h"

//#include <QtGui>
#include <QDomDocument>
#include <QFile>
#include <QDir>
#include <QTimer>
#include <QTextStream>
#include <QSettings>

#include "sscTime.h"
#include "sscMessageManager.h"
#include "sscDataManager.h"
#include "sscToolManager.h"
#include "sscUtilHelpers.h"
#include "sscToolManager.h"

#include "cxViewManager.h"
#include "cxRepManager.h"
#include "cxFileCopied.h"
#include "cxDataLocations.h"
#include "cxRegistrationManager.h"
#include "cxStateMachineManager.h"

namespace cx
{

PatientData::PatientData() :
    mSettings(DataLocations::getSettings())
{
}

QString PatientData::getActivePatientFolder() const
{
  return mActivePatientFolder;
}

bool PatientData::isPatientValid() const
{
  //ssc::messageManager()->sendDebug("PatientData::isPatientValid: "+string_cast(!mActivePatientFolder.isEmpty()));
  return !mActivePatientFolder.isEmpty();
}

void PatientData::setActivePatient(const QString& activePatientFolder)
{
  if(activePatientFolder == mActivePatientFolder)
    return;

  mActivePatientFolder = activePatientFolder;
  //TODO
  //Update gui in some way to show which patient is active

  emit patientChanged();
}

void PatientData::newPatient(QString choosenDir)
{
  this->clearPatient();
  createPatientFolders(choosenDir);
  this->setActivePatient(choosenDir);
}

/**Remove all data referring to the current patient from the system,
 * enabling us to load new patient data.
 */
void PatientData::clearPatient()
{
  ssc::dataManager()->clear();
  ssc::toolManager()->clear();
  viewManager()->clear();
  registrationManager()->clear();
  //rep
  //usrec?

  QString patientDatafolder = mSettings->value("globalPatientDataFolder").toString();

  this->setActivePatient(patientDatafolder + "/NoPatient");
}

//void PatientData::loadPatientFileSlot()
void PatientData::loadPatient(QString choosenDir)
{
  this->clearPatient();
  //ssc::messageManager()->sendDebug("loadPatient() choosenDir: "+string_cast(choosenDir));
  if (choosenDir == QString::null)
    return; // On cancel
  
  QFile file(choosenDir+"/custusdoc.xml");
  if(file.open(QIODevice::ReadOnly))
  {
    QDomDocument doc;
    QString emsg;
    int eline, ecolumn;
    // Read the file
    if (!doc.setContent(&file, false, &emsg, &eline, &ecolumn))
    {
      ssc::messageManager()->sendError("Could not parse XML file :"
                                 +file.fileName()+" because: "
                                 +emsg+"");
    }
    else
    {
      //Read the xml
      this->readLoadDoc(doc, choosenDir);
    }
    file.close();
  }
  else //User have created the directory create xml file and folders
  {
    //TODO: Ask the user if he want to convert the folder
    ssc::messageManager()->sendInfo("Found no CX3 data in folder: " +
                                    choosenDir +
                                    " Converting the folder to a patent folder...");
    createPatientFolders(choosenDir);
  }
  
  this->setActivePatient(choosenDir);
}

void PatientData::savePatient()
{

  if(mActivePatientFolder.isEmpty())
  {
//    ssc::messageManager()->sendWarning("No patient selected, select or create patient before saving!");
//    this->newPatientSlot();
    return;
  }

  //Gather all the information that needs to be saved
  QDomDocument doc;
  this->generateSaveDoc(doc);

  QFile file(mActivePatientFolder + "/custusdoc.xml");
  if(file.open(QIODevice::WriteOnly | QIODevice::Truncate))
  {
    QTextStream stream(&file);
    stream << doc.toString(4);
    file.close();
    ssc::messageManager()->sendInfo("Created "+file.fileName());
  }
  else
  {
    ssc::messageManager()->sendError("Could not open "+file.fileName()
                               +" Error: "+file.errorString());
  }

  ssc::toolManager()->savePositionHistory();
  //Write the data to file, fx modified images... etc...
  //TODO Implement when we know what we want to save here...
}

bool PatientData::copyFile(QString source, QString dest)
{
  if (source==dest)
    return true;

  QFileInfo info(dest);

  if (info.exists())
  {
    ssc::messageManager()->sendWarning("File already exists: "+dest+", copy skipped.");
    return true;
  }

  QDir().mkpath(info.path());

  QFile toFile(dest);
  if(QFile(source).copy(toFile.fileName()))
  {
    //messageMan()->sendInfo("File copied to new location: "+pathToNewFile.toStdString());
  }
//  else
//  {
//    ssc::messageManager()->sendError("First copy failed!");
//    return false;
//  }
  if(!toFile.flush())
  {
    ssc::messageManager()->sendWarning("Failed to copy file: "+source);
    return false;
  }
  if(!toFile.exists())
  {
    ssc::messageManager()->sendWarning("File not copied: " + source);
    return false;
  }

  ssc::messageManager()->sendInfo("Copied " + source + " -> " + dest);


  return true;
}

/**Copy filename and all files with the same name (and different extension)
 * to destFolder.
 *
 */
bool PatientData::copyAllSimilarFiles(QString fileName, QString destFolder)
{
  QDir sourceFolder(QFileInfo(fileName).path());
  QStringList filter;
  filter << QFileInfo(fileName).completeBaseName() + ".*";
  QStringList sourceFiles = sourceFolder.entryList(filter, QDir::Files);
//  std::cout << "found files: " << sourceFiles.join(" ") << std::endl;;

  for (int i=0; i<sourceFiles.size(); ++i)
  {
    QString sourceFile = sourceFolder.path() + "/" + sourceFiles[i];
    QString destFile = destFolder + "/" + QFileInfo(sourceFiles[i]).fileName();
    this->copyFile(sourceFile, destFile);
  }

  return true;
}


ssc::DataPtr PatientData::importData(QString fileName)
{
  //ssc::messageManager()->sendDebug("PatientData::importData() called");
//  this->savePatientFileSlot();

//  QString fileName = QFileDialog::getOpenFileName( this,
//                                  QString(tr("Select data file")),
//                                  mSettings->value("globalPatientDataFolder").toString(),
//                                  tr("Image/Mesh (*.mhd *.mha *.stl *.vtk)"));
  if(fileName.isEmpty())
  {
    ssc::messageManager()->sendInfo("Import canceled");
    return ssc::DataPtr();
  }
  
  QString patientsImageFolder = mActivePatientFolder+"/Images/";
//  QString patientsSurfaceFolder = mActivePatientFolder+"/Surfaces/";

  QFileInfo fileInfo(fileName);
  QString fileType = fileInfo.suffix();
  QString pathToNewFile = patientsImageFolder+fileInfo.fileName();
  QFile fromFile(fileName);
  QString strippedFilename = ssc::changeExtension(fileInfo.fileName(), "");
  QString uid = strippedFilename+"_"+fileInfo.created().toString(ssc::timestampSecondsFormat());

  if (ssc::dataManager()->getData(uid))
  {
    ssc::messageManager()->sendWarning("Data with uid "+ uid + " already exists. Impoirt cancelled.");
    return ssc::DataPtr();
  }

  // Read files before copy
  ssc::DataPtr data = ssc::dataManager()->loadData(uid, fileName, ssc::rtAUTO);

  data->setShading(true);

  QDir patientDataDir(mActivePatientFolder);

  data->setFilePath(patientDataDir.relativeFilePath(pathToNewFile)); // Update file path

  this->copyAllSimilarFiles(fileName, patientsImageFolder);
//  ssc::messageManager()->sendDebug("Data is now copied into the patient folder!");

  this->savePatient();

  return data;
}

void PatientData::createPatientFolders(QString choosenDir)
{
  //ssc::messageManager()->sendDebug("PatientData::createPatientFolders() called");
  if(!choosenDir.endsWith(".cx3"))
    choosenDir.append(".cx3");

  ssc::messageManager()->sendInfo("Selected a patient to work with.");

  // Create folders
  if(!QDir().exists(choosenDir))
  {
    QDir().mkdir(choosenDir);
    ssc::messageManager()->sendInfo("Made a new patient folder: "+choosenDir);
  }

  QString newDir = choosenDir;
  newDir.append("/Images");
  if(!QDir().exists(newDir))
  {
    QDir().mkdir(newDir);
    ssc::messageManager()->sendInfo("Made a new image folder: "+newDir);
  }

  newDir = choosenDir;
  newDir.append("/Surfaces");
  if(!QDir().exists(newDir))
  {
    QDir().mkdir(newDir);
    ssc::messageManager()->sendInfo("Made a new surface folder: "+newDir);
  }

  newDir = choosenDir;
  newDir.append("/Logs");
  if(!QDir().exists(newDir))
  {
    QDir().mkdir(newDir);
    ssc::messageManager()->sendInfo("Made a new logging folder: "+newDir);
  }

  this->savePatient();
}


/**
 * Xml version 1.0: Knows about the nodes: \n
 * \<managers\> \n
 *   \<datamanager\> \n
 *     \<image\> \n
 *        \<uid\> //an images unique id \n
 *        \<name\> //an images name \n
 *        \<transferfunctions\> //an images transferefunction \n
 *            \<alpha\> //a transferefunctions alpha values \n
 *            \<color\> //a transferefunctions color values
 */
/**
 * Xml version 2.0: Knows about the nodes: \n
 * \<patient\> \n
 *  \<active_patient\> //relative path to this patients folder \n
 *  \<managers\> \n
 *     \<datamanager\> \n
 *       \<image\> \n
 *         \<uid\> //an images unique id \n
 *         \<name\> //an images name \n
 *         \<transferfunctions\> //an images transferefunction \n
 *            \<alpha\> //a transferefunctions alpha values \n
 *            \<color\> //a transferefunctions color values \n
 */
void PatientData::generateSaveDoc(QDomDocument& doc)
{
  doc.appendChild(doc.createProcessingInstruction("xml version =", "'1.0'"));

  QDomElement patientNode = doc.createElement("patient");
  QDomElement activePatientNode = doc.createElement("active_patient");
  activePatientNode.appendChild(doc.createTextNode(mActivePatientFolder.toStdString().c_str()));
  patientNode.appendChild(activePatientNode);
  doc.appendChild(patientNode);

  QDomElement managerNode = doc.createElement("managers");
  patientNode.appendChild(managerNode);

  ssc::dataManager()->addXml(managerNode);
  ssc::toolManager()->addXml(managerNode);
  viewManager()->addXml(managerNode);
  registrationManager()->addXml(managerNode);
  stateManager()->addXml(managerNode);

  //ssc::messageManager()->sendInfo("Xml file ready to be written to disk.");
}
void PatientData::readLoadDoc(QDomDocument& doc, QString patientFolder)
{
  //ssc::messageManager()->sendDebug("PatientData::readLoadDoc() called");
  //Get all the nodes
  QDomNode patientNode = doc.namedItem("patient");
  QDomNode managerNode = patientNode.namedItem("managers");

  //Evaluate the xml nodes and load what's needed
  QDomNode dataManagerNode = managerNode.namedItem("datamanager");
  /*if(!patientNode.isNull())
  {
    QDomElement activePatientNode = patientNode.namedItem("active_patient").toElement();
    if(!activePatientNode.isNull())
    {
      ssc::messageManager()->sendDebugs("Active patient node is"
                                +mActivePatientFolder.toStdString());
    }
  }*/
  if (!dataManagerNode.isNull())
  {
    ssc::dataManager()->parseXml(dataManagerNode, patientFolder);
  }

  QDomNode toolmanagerNode = managerNode.namedItem("toolManager");
  ssc::toolManager()->parseXml(toolmanagerNode);

  QDomNode viewmanagerNode = managerNode.namedItem("viewManager");
  viewManager()->parseXml(viewmanagerNode);

  QDomNode registrationNode = managerNode.namedItem("registrationManager");
  registrationManager()->parseXml(registrationNode);

  QDomNode stateManagerNode = managerNode.namedItem("stateManager");
  stateManager()->parseXml(stateManagerNode);
}


} // namespace cx

