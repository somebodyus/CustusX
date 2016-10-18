#ifndef SHADERCALLBACK_H
#define SHADERCALLBACK_H

#include <vtkCommand.h>

// -----------------------------------------------------------------------
// Update a uniform in the shader for each render. We do this with a
// callback for the UpdateShaderEvent
class ShaderCallback : public vtkCommand
{
public:
  static ShaderCallback *New();

  virtual void Execute(vtkObject *, unsigned long event, void*cbo);

  ShaderCallback();
};


#endif // SHADERCALLBACK_H