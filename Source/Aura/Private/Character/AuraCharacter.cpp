// zhuz1's learning project


#include "Character/AuraCharacter.h"

#include "GameFramework/CharacterMovementComponent.h"

AAuraCharacter::AAuraCharacter()
{
	// 这行代码设置角色的旋转方向以匹配其移动方向。当这个属性被设置为 true 时，角色会自动朝向它正在移动的方向。 
	GetCharacterMovement()->bOrientRotationToMovement = true;
	//  这行代码设置了角色旋转的速率。这里的 FRotator 是一个旋转值，分别代表绕着X轴、Y轴和Z轴的旋转速度。
	//  在这个例子中，设置的是绕Y轴（通常是上下旋转）旋转400度每秒，而绕X轴和Z轴的旋转速度则是0。
	GetCharacterMovement()->RotationRate = FRotator(0.f,400.f,0.f);
	// 这行代码确保角色的移动被限制在一个平面内。这通常用于2D游戏或需要限制角色不在三维空间中上下移动的场景。
	GetCharacterMovement()->bConstrainToPlane = true;
	// 当这个属性设置为 true 时，角色将在游戏开始时自动对齐到约束平面上。这对于确保角色始终保持在特定的游戏平面上很有帮助。
	GetCharacterMovement()->bSnapToPlaneAtStart = true;
	// 这行代码设置角色不使用控制器（如游戏手柄或键盘）的俯仰（pitch，即上下）旋转。这意味着角色的上下朝向不会根据控制器的俯仰输入改变。
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	/*
	*是的，控制器的旋转通常指的是输入设备（如游戏手柄或鼠标）提供的旋转输入。在虚幻引擎（Unreal Engine）的上下文中，这涉及到几个方面：
	1. **俯仰（Pitch）**: 控制器俯仰通常指的是视角或角色的上下移动。在游戏手柄上，这可能由摇杆的上下移动控制；在使用鼠标的情况下，通常是鼠标的前后移动。
	2. **偏航（Yaw）**: 控制器偏航指的是左右转向。在游戏手柄上，这通常是通过摇杆的左右移动来控制的；在鼠标控制的情况下，通常是鼠标的左右移动。
	3. **翻滚（Roll）**: 控制器翻滚较少用于标准游戏控制，通常指的是围绕前进方向的旋转。在某些飞行模拟游戏中可能会用到。
	在您的代码段中，将 `bUseControllerRotationPitch`、`bUseControllerRotationYaw` 和 `bUseControllerRotationRoll` 都设置为 `false`
	意味着角色的这些方向的旋转不会根据控制器的输入来改变。
	这通常用于创建角色自动朝向其移动方向的效果，而不是由玩家直接控制其朝向。
	这种设置在第三人称游戏中非常常见，其中角色的移动和方向更多地由游戏逻辑控制，而不是直接通过玩家的输入。
	 */
	
}
