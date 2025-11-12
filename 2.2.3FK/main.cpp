#include "GlfwUse.h"
#include "OpenGLWithGladGlfw.h"
#include "GlfwInputCollector.h"
#include "ImguiWithOpenglGlfw.h"
#include "BoneIndicatorUse.h"

#include "shader.h"
#include "model.h"
#include "Posture.h"
#include "indicator.h"
#include "animation.h"
#include "animator.h"
#include "camera.h"
#include "ProcessInput.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <vector>
#include <array>
int main()
{
	//创建Glfw实例
	GlfwUse glfwUse{ 4,3,1920,1080,"FK" };
	//glfw操作器实例
	GlfwInputCollector inputCollector{ *glfwUse.ptr_window };
	//创建opengl实例
	OpenGLWithGladGlfw openglUse{ glfwUse };
	//窗口上下文初始化
	glfwUse.windowContextInitializeAndSetCallbacks(openglUse, inputCollector,glfwUse);
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	stbi_set_flip_vertically_on_load(true);
	ImguiWithOpenglGlfw imguiUse{ glfwUse,inputCollector };
	//着色器
	Shader modelAnimShader("skinning.vert", "texture.frag");
	Shader objectShader("regularOBJ.vert", "texture.frag");
	Shader quadShader("quadrangle.vert", "texture.frag");
	//模型
	Model manModel("resources/Capoeira_Mannequin.dae");
	Model floorModel("floor/floor.obj");
	//动画
	Animation danceAnimation(("resources/Capoeira_Mannequin.dae"), &manModel);
	Animator animator(&danceAnimation);
	//FK
	Posture manPosture(&manModel);
	//骨骼指示器
	Indicator indicator;
	indicator.loadTextureSelected("Indicator/selected.png");
	indicator.loadTextureUnselected("Indicator/unselected.png");
	BoneIndicatorUse indicatorUse{};
	//模型的骨骼列表
	auto BoneIDtoNames = manModel.GetBoneIDtoNames();//?后面会用到，但是放在这里不太漂亮，后面建议移动一下
	//相机
	Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
	ProcessInput processInput(inputCollector, glfwUse, imguiUse, openglUse, camera, indicatorUse,manPosture);
	//主循环
	while (!glfwWindowShouldClose(glfwUse.ptr_window))
	{
		//更新事件
		inputCollector.update();
		imguiUse.ifIoWantUseMouse();
		glfwPollEvents();
		processInput.update();//因为cursoreMovedInThisFrame的更新机制，glfwPollEvents必须在processInput.update前面，这个得优化
		glfwUse.startTimer();
		//processInput(window);
		openglUse.setClearColorAndClear(0.2f, 0.5f, 0.9f, 1.0f);
		openglUse.setAspectRatio();
		//设置三矩阵
		glm::mat4 projectionMatrix = glm::perspective(glm::radians(camera.Zoom), openglUse.aspectRatio, 0.1f, 100.0f);
		glm::mat4 viewMatrix = camera.GetViewMatrix();
		glm::mat4 modelMatrix = glm::mat4(1.0f);

		//绘制模型站姿== == == == == == == == == == == == == == 
		//更新条件
		if (manPosture.rotationMode)
		{
			manPosture.processRotationInput(glfwUse.ptr_window, manPosture, indicatorUse);
		}

		modelAnimShader.use();
		modelAnimShader.setMat4("projection", projectionMatrix);
		modelAnimShader.setMat4("view", viewMatrix);
		auto &postureLocal = manPosture.GetFinalLocalBoneMatrices();
		for (int i = 0; i < postureLocal.size(); ++i)//
			modelAnimShader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", postureLocal[i]);//计算FinalTransform
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(1.0f));
		modelAnimShader.setMat4("model", modelMatrix);
		manModel.Draw(modelAnimShader);
		//绘制地板== == == == == == == == == == == == == == 
		objectShader.use();
		objectShader.setMat4("projection", projectionMatrix);
		objectShader.setMat4("view", viewMatrix);
		modelMatrix = glm::mat4(1.0);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, 0.0f, 0.0f));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(3.0f));
		objectShader.setMat4("model", modelMatrix);
		floorModel.Draw(objectShader);
		//绘制骨骼指示器== == == == == == == == == == == == == ==  
		glDisable(GL_DEPTH_TEST);

		auto &m_finalGlobalBoneMatrices = manPosture.GetFinalGlobalBoneMatrices();
		std::vector<std::array<glm::vec3, 2>> bonePositionWindowCoords;//也是boneinfo中的id的顺序
		float boneMarkerScale = 0.02f;//骨骼节点大小

		for (unsigned int id = 0; id < m_finalGlobalBoneMatrices.size(); id++)
		{
			quadShader.use();
			glm::vec3 bonePosition = glm::vec3(0.0f);
			bonePosition.x = m_finalGlobalBoneMatrices[id][3][0];
			bonePosition.y = m_finalGlobalBoneMatrices[id][3][1];
			bonePosition.z = m_finalGlobalBoneMatrices[id][3][2];

			modelMatrix = glm::mat4(1.0f);
			modelMatrix = glm::scale(modelMatrix, glm::vec3(1.0));//适配人物大小
			modelMatrix = glm::translate(modelMatrix, bonePosition);
			modelMatrix = glm::scale(modelMatrix, glm::vec3(boneMarkerScale));

			quadShader.setMat4("projection", projectionMatrix);
			quadShader.setMat4("view", viewMatrix);
			quadShader.setMat4("model", modelMatrix);
			quadShader.setVec3("cameraUp", camera.Up);
			quadShader.setVec3("cameraRight", camera.Right);

			quadShader.setInt("texture_diffuse1", 0);

			glBindVertexArray(indicator.quadVAO);
			glActiveTexture(GL_TEXTURE0);
			//高亮骨骼
			if (id == indicatorUse.hoveredBoneID //绘制到被悬浮的骨骼ID
				|| id == indicatorUse.chosenBoneID)//绘制到被选中的骨骼ID
				glBindTexture(GL_TEXTURE_2D, indicator.texSelected);
			//常亮骨骼
			else
				glBindTexture(GL_TEXTURE_2D, indicator.texUnselected);
			//绘制骨骼
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			//骨骼绘制完后，储存对应的屏幕坐标== == == == == == == == == == == == == == 
			//顶点世界坐标转窗口坐标
			float modelScaleX = glm::length(glm::vec3(modelMatrix[0]));
			float modelScaleY = glm::length(glm::vec3(modelMatrix[1]));
			glm::vec3 modelPosition = glm::vec3(glm::vec3(modelMatrix[3]));

			glm::vec3 topLeft_World = modelPosition + (camera.Right * (-0.5f * modelScaleX)) + (camera.Up * (0.5f * modelScaleY));
			glm::vec3 bottomRight_World = modelPosition + (camera.Right * (0.5f * modelScaleX)) + (camera.Up * (-0.5f * modelScaleY));

			glm::vec3 winCoordTopLeft = glm::project(topLeft_World, viewMatrix, projectionMatrix, openglUse.viewportSize);
			glm::vec3 windCoordbottomRight = glm::project(bottomRight_World, viewMatrix, projectionMatrix, openglUse.viewportSize);
			winCoordTopLeft.y = openglUse.viewportSize.w - winCoordTopLeft.y;//反转y轴成向下递增
			windCoordbottomRight.y = openglUse.viewportSize.w - windCoordbottomRight.y;//反转y轴成向下递增
			bonePositionWindowCoords.push_back({ winCoordTopLeft,windCoordbottomRight });

			//鼠标悬浮于当前绘制的骨骼
			if (!inputCollector.toCaptureCursor//鼠标模式
				&& (inputCollector.xPosLastFrame > winCoordTopLeft.x && inputCollector.yPosLastFrame > winCoordTopLeft.y 
					&& inputCollector.xPosLastFrame < windCoordbottomRight.x && inputCollector.yPosLastFrame < windCoordbottomRight.y))
			{
				indicatorUse.hoveringOverBone = true;
				indicatorUse.hoveredBoneID = id;
			}
		}
		//鼠标从之前悬浮的骨骼上移开
		if (!inputCollector.toCaptureCursor//鼠标模式
			&& indicatorUse.hoveredBoneID != -1//曾悬浮于任何骨骼
			&& !(inputCollector.xPosLastFrame > bonePositionWindowCoords[indicatorUse.hoveredBoneID][0].x 
				&& inputCollector.yPosLastFrame > bonePositionWindowCoords[indicatorUse.hoveredBoneID][0].y
				&& inputCollector.xPosLastFrame < bonePositionWindowCoords[indicatorUse.hoveredBoneID][1].x
				&& inputCollector.yPosLastFrame < bonePositionWindowCoords[indicatorUse.hoveredBoneID][1].y
				)
			)
		{
			indicatorUse.hoveringOverBone = false;
			indicatorUse.hoveredBoneID = -1;
		}

		glEnable(GL_DEPTH_TEST);
		//imgui
		imguiUse.newFrame("测试中文");
		ImGui::Text("延迟 %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		//鼠标位置
		ImGui::Text("鼠标当前位置: X: %.2f  Y: %.2f", inputCollector.xPosLastFrame, inputCollector.yPosLastFrame);
		ImGui::Text("鼠标上一帧位置: X: %.2f  Y: %.2f", inputCollector.xPosCurrentFrame, inputCollector.yPosCurrentFrame);
		ImGui::Text("OFFSET: X: %.2f  Y: %.2f", inputCollector.xPosOffsetBetweenTwoFrames, inputCollector.yPosOffsetBetweenTwoFrames);
		//ImGui::Text("cursoreMovedInThisFrame: %s", inputCollector.cursoreMovedInThisFrame ? "true" : "false");
		//骨骼位置显示
		if (indicatorUse.hoveringOverBone)
		{
			ImGui::Text("骨骼名称: %s  ID:%i", BoneIDtoNames[indicatorUse.hoveredBoneID].c_str(), indicatorUse.hoveredBoneID);
			ImGui::Text("TopLeft       X: %.2f    Y: %.2f", bonePositionWindowCoords[indicatorUse.hoveredBoneID][0].x, bonePositionWindowCoords[indicatorUse.hoveredBoneID][0].y);
			ImGui::Text("BottmRight X: %.2f    Y: %.2f", bonePositionWindowCoords[indicatorUse.hoveredBoneID][1].x, bonePositionWindowCoords[indicatorUse.hoveredBoneID][1].y);
		}
		else
		{
			ImGui::Text("骨骼名称: NONE");
			ImGui::Text("TopLeft       X: NONE    Y: NONE");
			ImGui::Text("BottmRight X: NONE    Y: NONE");
		}
		//按键与功能状态提示
		ImGui::Text("按键与功能状态提示:");
		ImGui::Text("按键:");
		ImGui::Text("leftClickCurrentIsDown: %s", inputCollector.leftClickCurrentIsDown ? "true" : "false");
		ImGui::Text("leftClickJustPressed: %s", inputCollector.leftClickJustPressed ? "true" : "false");
		ImGui::Text("leftClickJustReleased: %s", inputCollector.leftClickJustReleased ? "true" : "false");
		ImGui::Text("R_KeyCurrentIsDown: %s", inputCollector.R_KeyCurrentIsDown ? "true" : "false");
		ImGui::Text("R_KeyJustPressed: %s", inputCollector.R_KeyJustPressed ? "true" : "false");
		ImGui::Text("R_KeyJustReleased: %s", inputCollector.R_KeyJustReleased ? "true" : "false");
		

		ImGui::Text("toCaptureCursor: %s", inputCollector.toCaptureCursor ? "true" : "false");
		ImGui::Text("enableClickInput: %s", inputCollector.enableClickInput ? "true" : "false");
		ImGui::Text("ESCAPE_KeyCurrentIsDown: %s", inputCollector.ESCAPE_KeyCurrentIsDown ? "true" : "false");

		ImGui::Text("功能状态:");
		ImGui::Text("hoveringOverBone:  %s", indicatorUse.hoveringOverBone ? "true" : "false");
		ImGui::Text("hoveredBoneID:  %i", indicatorUse.hoveredBoneID);

		ImGui::Text("chosingBone:  %s", indicatorUse.chosingBone ? "true" : "false");
		ImGui::Text("chosenBoneID:  %i", indicatorUse.chosenBoneID);

		ImGui::Text("rotationMode:  %s", manPosture.rotationMode ? "true" : "false");
		imguiUse.endFrame();
		glfwSwapBuffers(glfwUse.ptr_window);
	}
	imguiUse.terminate();
	glfwTerminate();
	return 0;
}