#include <stdgl/ShaderProgram.hpp>
#include <stdgl/VoxelData.hpp>
#include <stdgl/Texture3D.hpp>
#include <csgframework/CsgTree.hpp>
#include <csgframework/CsgLoader.hpp>

#include <stdio.h>
#include <iostream>
#include <memory>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <GLFW/glfw3.h>

#include <Eigen/Geometry>

#define TO_STRING(x) #x

#define SHADER_PREFIX \
  "#version 120\n" \
  "#define MAXFLOAT 1.0e15f\n"

const char* vs_str = SHADER_PREFIX TO_STRING
(
  //! Normalized pixel coordinates.
  varying vec2 vPixel;

  void main (void)
  {
    vPixel = gl_Vertex.xy;

    gl_Position = gl_Vertex;
  }
);

const char* fs_str = SHADER_PREFIX TO_STRING
(
  //! Normalized pixel coordinates.
  varying vec2 vPixel;

  uniform vec3 iResolution;
  uniform float iGlobalTime;
  uniform float iCameraAngle;
  uniform float iCameraDistance;

  uniform float iSceneRadius;

  //! Scene epsilon to prevent self-intersections.
  uniform float iSceneEpsilon;

  uniform sampler3D iDistanceTexture;

  uniform vec3 iBoundsMin;
  uniform vec3 iBoundsMax;

  float sdPlane (vec3 p)
  {
    return p.y;
  }

  //----------------------------------------------------------------------

  vec2 opU (vec2 d1, vec2 d2)
  {
    return (d1.x < d2.x) ? d1 : d2;
  }

  //----------------------------------------------------------------------

  float distanceField (in vec3 thePoint)
  {
    vec3 aTexCoord = (thePoint - iBoundsMin) / (iBoundsMax - iBoundsMin);

    return texture3D (iDistanceTexture, aTexCoord).r;
  }


  vec2 map (in vec3 thePos)
  {
    vec2 res = vec2 (distanceField (thePos), 46.9);
    res = opU (vec2 (sdPlane (thePos - vec3( 0.0, iBoundsMin.y, 0.0)), 1.0), res);

    return res;
  }

  vec2 castRayTexture (in vec3 ro, in vec3 rd, in vec2 theRange)
  {
    float tmin = theRange.x;
    float tmax = theRange.y;

    float precis = iSceneEpsilon;
    float t = tmin;
    float m = -1.0;

    for (int i = 0; i < 100; i++)
    {
      vec3 aPos = ro + rd * t;
      vec2 res = map (aPos);

      if (res.x < precis || t > tmax)
      {
        break;
      }

      t += res.x;
      m = res.y;
    }

    if (t > tmax)
    {
      m = -1.0;
    }

    return vec2 (t, m);
  }

  float softshadow (in vec3 ro, in vec3 rd, in float mint, in float tmax)
  {
    float res = 1.0;
    float t = mint;

    for (int i = 0; i < 16; i++)
    {
      float h = map (ro + rd * t).x;
      res = min (res, 8.0 * h / t);
      t += clamp (h, 0.02, 0.10);

      if (h < iSceneEpsilon || t > tmax)
      {
        break;
      }
    }

    return clamp (res, 0.0, 1.0);
  }

  vec3 calcNormal (in vec3 pos)
  {
    vec3 eps = vec3 (iSceneEpsilon, 0.0, 0.0);
    vec3 nor = vec3 ( map (pos + eps.xyy).x - map (pos - eps.xyy).x,
                      map (pos + eps.yxy).x - map (pos - eps.yxy).x,
                      map (pos + eps.yyx).x - map (pos - eps.yyx).x);
    return normalize (nor);
  }

  float calcAO (in vec3 pos, in vec3 nor)
  {
    float occ = 0.0;
    float sca = 1.0;

    for (int i = 0; i < 5; i++)
    {
      float hr = 0.01 + 0.12 * float (i) / 4.0;
      vec3 aopos =  nor * hr + pos;
      float dd = map (aopos).x;
      occ += - (dd - hr) * sca;
      sca *= 0.95;
    }

    return clamp (1.0 - 3.0 * occ, 0.0, 1.0);
  }

  vec3 render (in vec3 ro, in vec3 rd, in vec2 theRange)
  {
    vec3 col = vec3 (0.7, 0.9, 1.0) + rd.y * 0.8;

    if (theRange.x == MAXFLOAT)
    {
      return col;
    }

    vec2 res = castRayTexture (ro, rd, theRange);

    float t = res.x;
    float m = res.y;

    if (m > -0.5)
    {
      vec3 pos = ro + t * rd;
      vec3 nor = calcNormal (pos);
      vec3 ref = reflect (rd, nor);

      // col = abs(nor);
      // return col;

      // material
      col = 0.45 + 0.3 * sin (vec3 (0.05, 0.08, 0.10) * (m - 1.0));

      if (m < 1.5)
      {
        float f = mod (floor (5.0 * pos.z) + floor (5.0 * pos.x), 2.0);
        col = 0.4 + 0.1 * f * vec3 (1.0);
      }

      // lighitng
      float occ = calcAO (pos, nor);
      vec3  lig = normalize (vec3 (-0.6, 0.7, -0.5));
      float amb = clamp (0.5 + 0.5 * nor.y, 0.0, 1.0);
      float dif = clamp (dot (nor, lig), 0.0, 1.0);
      float bac = clamp (dot (nor, normalize (vec3 (-lig.x, 0.0, -lig.z))), 0.0, 1.0) * clamp (1.0 - pos.y, 0.0, 1.0);
      float dom = smoothstep (-0.1, 0.1, ref.y);
      float fre = pow (clamp (1.0 + dot (nor, rd), 0.0, 1.0), 2.0);
      float spe = pow (clamp (dot (ref, lig), 0.0, 1.0), 16.0);

      // dif *= softshadow (pos, lig, 0.02, 2.5);
      // dom *= softshadow( pos, ref, 0.02, 2.5 );

      vec3 lin = vec3 (0.0);
      lin += 1.20 * dif * vec3 (1.00, 0.85, 0.55);
      lin += 1.20 * spe * vec3 (1.00, 0.85, 0.55) * dif;
      lin += 0.20 * amb * vec3 (0.50, 0.70, 1.00) * occ;
      lin += 0.30 * dom * vec3 (0.50, 0.70, 1.00) * occ;
      lin += 0.30 * bac * vec3 (0.25, 0.25, 0.25) * occ;
      lin += 0.40 * fre * vec3 (1.00, 1.00, 1.00) * occ;
      col = col * lin;
    }

    return vec3 (clamp (col, 0.0, 1.0));
  }

  mat3 setCamera (in vec3 ro, in vec3 ta, float cr)
  {
    vec3 cw = normalize (ta - ro);
    vec3 cp = vec3 (sin (cr), cos (cr), 0.0);
    vec3 cu = normalize (cross (cw, cp));
    vec3 cv = normalize (cross (cu, cw));
    return mat3 (cu, cv, cw);
  }

  vec2 intersectBox (in vec3 theOrigin, in vec3 theDirect, in vec3 theMinPnt, in vec3 theMaxPnt)
  {
    vec3 aTimeBoxMin = (theMinPnt - theOrigin) * (1.f / theDirect);
    vec3 aTimeBoxMax = (theMaxPnt - theOrigin) * (1.f / theDirect);

    vec3 aTimeMax = max (aTimeBoxMin, aTimeBoxMax);
    vec3 aTimeMin = min (aTimeBoxMin, aTimeBoxMax);

    float aTime1 = max (aTimeMin.x, max (aTimeMin.y, aTimeMin.z));
    float aTime2 = min (aTimeMax.x, min (aTimeMax.y, aTimeMax.z));

    return aTime1 > aTime2 || aTime2 < 0.f ?
           vec2 (MAXFLOAT) : vec2 (max (aTime1, 0.f), aTime2);
  }

  void main()
  {

    vec2 p = vPixel;
    p.x *= iResolution.x / iResolution.y;

    float time = 15.0f + iGlobalTime;

    // camera
    vec3 ta = (iBoundsMin + iBoundsMax) * 0.5f;
    float R = iCameraDistance;
    vec3 ro = ta + vec3 (R * cos (0.1f * time + iCameraAngle),
                         R * 0.5f,
                         R * sin (0.1f * time + iCameraAngle));

    // camera-to-world transformation
    mat3 ca = setCamera (ro, ta, 0.0f);

    // ray direction
    vec3 rd = ca * normalize (vec3 (p.xy, 2.0f));

    vec2 aRange = intersectBox (ro, rd, iBoundsMin, iBoundsMax);

    // render
    vec3 col = render (ro, rd, aRange);

    col = pow (col, vec3 (0.4545f));

    gl_FragColor = vec4 (col, 1.0f);
  }

);

#define MAXFLOAT 1.0e15f

namespace
{
  float evalPrimitiveDistance (const Vec4f& thePos, CsgPrimitiveNode* theNode)
  {
    float aDistance = MAXFLOAT;
    Vec3f aScaling;

    Mat4f aMatWithoutScale = theNode->Transform();
    for (int i = 0; i < 3; ++i)
    {
      float aScaleI = aMatWithoutScale.col (i).head<3>().norm();
      aScaling[i] = aScaleI;
      aMatWithoutScale.col (i).head<3>() *= 1.f / aScaleI;
    }

    Vec4f aTransformedPos = aMatWithoutScale.inverse() * thePos;
    aTransformedPos /= aTransformedPos.w();
    const Vec3f& aPoint = aTransformedPos.head<3>();

    switch (theNode->TypeID())
    {
      case CSG_SPHERE:
      {
        aDistance = aPoint.norm() - aScaling.x(); // only uniform scaling for spheres
        break;
      }
      case CSG_BOX:
      {
        Vec3f d = aPoint.cwiseAbs() - aScaling;
        aDistance = std::min (std::max (d.x(), std::max (d.y(), d.z())), 0.f)
                      + (d.cwiseMax (Vec3f (0.f, 0.f, 0.f))).norm();
        break;
      }
    }

    return aDistance;
  }

  float evalDistance (const Vec4f& thePos, CsgNode* theNode)
  {
    if (theNode->IsLeaf())
    {
      return evalPrimitiveDistance (thePos, static_cast<CsgPrimitiveNode*> (theNode));
    }

    float aDistance = MAXFLOAT;
    CsgOperationNode* anOpNode = static_cast<CsgOperationNode*> (theNode);

    switch (theNode->TypeID())
    {
      case CSG_OP_UNION:
      {
        aDistance = std::min (evalDistance (thePos, anOpNode->Child<0>()),
                              evalDistance (thePos, anOpNode->Child<1>()));
        break;
      }
      case CSG_OP_INTER:
      {
        aDistance = std::max (evalDistance (thePos, anOpNode->Child<0>()),
                              evalDistance (thePos, anOpNode->Child<1>()));
        break;
      }
      case CSG_OP_MINUS:
      {
        aDistance = std::max ( evalDistance (thePos, anOpNode->Child<0>()),
                              -evalDistance (thePos, anOpNode->Child<1>()));
        break;
      }
    }

    return aDistance;
  }

  static const GLfloat aQuadVertices[] = { -1.f, -1.f,  0.f,
                                           -1.f,  1.f,  0.f,
                                            1.f,  1.f,  0.f,
                                            1.f,  1.f,  0.f,
                                            1.f, -1.f,  0.f,
                                           -1.f, -1.f,  0.f
                                         };

  static void errorCallback (int error, const char* description)
  {
    fprintf (stderr, "Error %d: %s\n", error, description);
  }
}

int main (int, char**)
{
  // Setup window
  glfwSetErrorCallback (errorCallback);

  if (!glfwInit())
  {
    return 1;
  }

  // Load CSG-tree
  auto aExtTrsf = Eigen::Scaling (1.0f) * Eigen::AngleAxisf (M_PI * 0.5f, Vec3f (1.f, 0.f, 0.f));
  Eigen::Affine3f aSphereTransform = aExtTrsf * Eigen::Translation3f (0.f, 0.4f, 0.f) * Eigen::Scaling (0.35f);
  Eigen::Affine3f aBoxTransform;
  aBoxTransform = aExtTrsf * Eigen::Translation3f (0.f, -0.1f, 0.f) * Eigen::Scaling (0.5f, 0.2f, 0.4f);

  // std::unique_ptr<CsgNode> aTree (new CsgOperationNode (CSG_OP_UNION,
  //   new CsgPrimitiveNode (CSG_SPHERE, aSphereTransform.matrix()),
  //   new CsgPrimitiveNode (CSG_BOX, aBoxTransform.matrix())));

  json11::Json aData = csg::Parser::parse ("sample_cubes.csg");

  std::unique_ptr<CsgNode> aTree (CsgLoader::LoadTree (aData));

  aTree->InitializeBounds();

  // Init distance field
  VoxelData aDistanceFiled (84, 84, 84,
                            aTree->Bounds().CornerMin(),
                            aTree->Bounds().CornerMax());

  const float aMinPointX = aDistanceFiled.MinCorner.x() + 0.5f * aDistanceFiled.CellSize.x();
  const float aMinPointY = aDistanceFiled.MinCorner.y() + 0.5f * aDistanceFiled.CellSize.y();
  const float aMinPointZ = aDistanceFiled.MinCorner.z() + 0.5f * aDistanceFiled.CellSize.z();

  std::cout << aDistanceFiled.MinCorner.transpose() << std::endl;
  std::cout << aDistanceFiled.MaxCorner.transpose() << std::endl;

  Vec4f aQuery (0.f, 0.f, 0.f, 1.f);

  for (int aX = 0; aX < aDistanceFiled.SizeX; ++aX)
  {
    aQuery[0] = aMinPointX + aX * aDistanceFiled.CellSize.x();

    for (int aY = 0; aY < aDistanceFiled.SizeY; ++aY)
    {
      aQuery[1] = aMinPointY + aY * aDistanceFiled.CellSize.y();

      for (int aZ = 0; aZ < aDistanceFiled.SizeZ; ++aZ)
      {
        aQuery[2] = aMinPointZ + aZ * aDistanceFiled.CellSize.z();

        aDistanceFiled.Value (aX, aY, aZ) = evalDistance (aQuery, aTree.get());
      }
    }
  }

  // Setup window
  GLFWwindow* aWindow = glfwCreateWindow (1280, 720, "csgviewer", NULL, NULL);
  glfwMakeContextCurrent (aWindow);

  // Setup ImGui
  ImGui_ImplGlfw_Init (aWindow, true);

  bool show_test_window = false;
  ImVec4 clear_color = ImColor (114, 144, 154);

  ShaderProgram aProgram (vs_str, fs_str);

  if (!aProgram.infoLog().empty())
  {
    std::cout << aProgram.infoLog() << std::endl;
  }

  // Init 1-component 3D texture
  Texture3D aDistFieldTexture (1);

  aDistFieldTexture.SetFiltering (TextureFilterMode (GL_LINEAR, GL_LINEAR));

  if (!aDistFieldTexture.Init (aDistanceFiled.SizeX,
                               aDistanceFiled.SizeY,
                               aDistanceFiled.SizeZ,
                               aDistanceFiled.Data))
  {
    return 1;
  }

  const float aSceneRadius = (aDistanceFiled.MaxCorner - aDistanceFiled.MinCorner).norm();
  float aSceneEpsilon = aSceneRadius * 1e-3f;
  float aCameraAngle = 0.f;
  float aCameraDistance = aSceneRadius;
  float aCamDistFactor = 1.f;
  double aLastMouseX = -1.0, aLastMouseY = -1.0;

  // Setup screen-quad VBO
  GLuint aScreenQuadVbo;
  glGenBuffers (1, &aScreenQuadVbo);
  glBindBuffer (GL_ARRAY_BUFFER, aScreenQuadVbo);
  glBufferData (GL_ARRAY_BUFFER, sizeof (GLfloat) * 3 * 6, aQuadVertices, GL_STATIC_DRAW);
  glBindBuffer (GL_ARRAY_BUFFER, 0);

  // Main loop
  while (!glfwWindowShouldClose (aWindow))
  {
    glfwPollEvents();
    ImGui_ImplGlfw_NewFrame();

    ImGuiIO& io = ImGui::GetIO();

    if (!io.WantCaptureMouse)
    {
      if (io.MouseDown[0])
      {
        double aMouseX, aMouseY;
        glfwGetCursorPos (aWindow, &aMouseX, &aMouseY);

        if (aLastMouseX >= 0.0)
        {
          aCameraAngle += (aMouseX - aLastMouseX) * 0.03f;
        }

        aLastMouseX = aMouseX;
        aLastMouseY = aMouseY;
      }
      else
      {
        aLastMouseX = -1.0;
      }
    }

    {
      if (ImGui::Button ("Test Window"))
      {
        show_test_window ^= 1;
      }

      ImGui::DragFloat ("Scene epsilon", &aSceneEpsilon, 1e-4f);

      // Camera angle slider
      ImGui::DragFloat ("Camera angle", &aCameraAngle, 0.0f, 3.141592f);

      ImGui::SliderFloat ("Camera zoom", &aCamDistFactor, 0.1f, 3.f);
      ImGui::Text ("Camera distance: %f", aCameraDistance * aCamDistFactor);

      ImGui::Text ("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    }

    if (show_test_window)
    {
      ImGui::SetNextWindowPos (ImVec2 (650, 20), ImGuiSetCond_FirstUseEver);
      ImGui::ShowTestWindow (&show_test_window);
    }

    // Rendering
    int aDisplayWidth, aDisplayHeight;
    glfwGetFramebufferSize (aWindow, &aDisplayWidth, &aDisplayHeight);
    glViewport (0, 0, aDisplayWidth, aDisplayHeight);
    glClear (GL_COLOR_BUFFER_BIT);

    // Draw screen quad
    glUseProgram (aProgram.programId());

    // Set uniforms
    glUniform3f (glGetUniformLocation (aProgram.programId(), "iResolution"), aDisplayWidth, aDisplayHeight, 0.f);
    glUniform1f (glGetUniformLocation (aProgram.programId(), "iGlobalTime"), glfwGetTime());
    glUniform1f (glGetUniformLocation (aProgram.programId(), "iCameraAngle"), aCameraAngle);
    glUniform1f (glGetUniformLocation (aProgram.programId(), "iCameraDistance"), aCameraDistance * aCamDistFactor);
    glUniform1ui (glGetUniformLocation (aProgram.programId(), "iDistanceTexture"), 0u);
    glUniform3fv (glGetUniformLocation (aProgram.programId(), "iBoundsMin"), 1, reinterpret_cast<GLfloat*> (&aDistanceFiled.MinCorner));
    glUniform3fv (glGetUniformLocation (aProgram.programId(), "iBoundsMax"), 1, reinterpret_cast<GLfloat*> (&aDistanceFiled.MaxCorner));
    glUniform1f (glGetUniformLocation (aProgram.programId(), "iSceneRadius"), aSceneRadius);
    glUniform1f (glGetUniformLocation (aProgram.programId(), "iSceneEpsilon"), aSceneEpsilon);

    aDistFieldTexture.Bind (GL_TEXTURE0);
    glBindBuffer (GL_ARRAY_BUFFER, aScreenQuadVbo);
    glEnableClientState (GL_VERTEX_ARRAY);
    glVertexPointer (3, GL_FLOAT, 0, NULL);
    glDrawArrays (GL_TRIANGLES, 0, 6);
    glDisableClientState (GL_VERTEX_ARRAY);
    glBindBuffer (GL_ARRAY_BUFFER, 0);
    aDistFieldTexture.Unbind (GL_TEXTURE0);
    glUseProgram (0);

    // Draw ImGui stuff
    glActiveTexture (GL_TEXTURE0);
    ImGui::Render();
    glfwSwapBuffers (aWindow);
  }

  glDeleteBuffers (1, &aScreenQuadVbo);

  // Cleanup
  ImGui_ImplGlfw_Shutdown();
  glfwTerminate();

  return 0;
}
