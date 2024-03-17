# ActionRoguelike

学习教程来自[Tom Looman]((https://github.com/tomlooman))的ActionRogueLike 课程，游戏类型为第三人称动作射击。使用C++/蓝图。
截止目前，本项目实现内容包含：

**角色相关：**

- 结合C++和动画状态机实现人物基础移动、冲刺，以及自制的攀爬系统

**场景物品：**

- 实现多种交互场景物品：炸药桶、宝箱、机关等

**UMG：**

- 玩家血条，交互tips，分数显示与简易更新动画，AI血条

**GameMode：**

- 敌人刷新，管理PlayerState，重生玩家

**敌人AI：**

- 根据与玩家距离，执行：随机移动或在玩家附近寻找玩家，若看见玩家则会自动移动至
攻击范围并攻击，可对玩家的攻击作出反应


The tutorial is from [Tom Looman](https://github.com/tomlooman)'s ActionRogueLike [course](https://courses.tomlooman.com/p/unrealengine-cpp?coupon_code=COMMUNITY15&src=github), and the game falls under the category of third-person action shooter. It utilizes C++/Blueprints.

Until now, this project includes the following implemented features:

**Character-related:**

- Third-person camera implementation
- Movement with WASD keys
- Sprinting and jumping
- Magic attack and black hole skill
- Implemented using Actor components

**Interaction with the Scene/Property Management/Skill Management:**

- GameMode controlling AI enemy spawns
- Granting player score rewards for kills
- Respawn of player character

**Enemy AI:**

- Based on the player's distance, the AI performs either random movement or searches for the player nearby.
- If the player is detected, the AI automatically moves into attack range and attacks.
- Responsive to player attacks.

**Scene Items:**

- Implementation of three interactive scene items: explosive barrels, treasure chests, and mechanisms.
- Interaction using defined interfaces.

**UMG (User Interface):**

- Player health bar
- Score display with simple update animations
- AI health bar (hidden when not under attack)
