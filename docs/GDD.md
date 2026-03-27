# Metal Metropolis - Game Design Document (GDD)

## Overview
**Title:** Metal Metropolis
**Project Type:** First game built on Elypso Engine (engine validation project)
**Scope:** Small, system-driven, replayable loop (not AAA, not content-heavy)
**Perspective:** First-person

---

## Core Concept
You are a scavenger sent into a dead, towering metal city buried in a desert.
Your goal is to collect scrap, survive the environment, and return before a sandstorm destroys you.

The city is vertical, unstable, and lifeless - only guarded by machines and shaped by harsh environmental conditions.

---

## Setting
- **Location:** Desolate desert planet
- **Primary Environment:** Abandoned metal city partially buried in sand
- **Structure:**
  - Ground level mostly buried
  - Traversal is primarily vertical
  - Buildings rise like metal spikes from the sand
- **Atmosphere:**
  - No organic life
  - Constant wind and environmental noise
  - Creaking metal structures
- **Key Constraint:** The only viable path is upward

---

## Narrative Premise
- You are part of a survivalist scavenger squad
- You are sent on a solo mission to investigate a signal
- Expected: small base
- Reality: massive dead city

- The city is long abandoned - no survivors remain
- Signs of sudden collapse are everywhere:
  - skeletons
  - bullet holes
  - torn and looted apartments
  - evidence of riots, conflict, and rapid evacuation
- Notes scattered throughout the city describe:
  - resource depletion
  - societal collapse
  - elites abandoning the population
  - those left behind descending into violence and hopelessness

- The signal originates from one of the highest apartments
  - an SOS or automated distress signal
  - whoever sent it is long gone
  - the signal is an echo, not a rescue opportunity

- The environment reinforces isolation:
  - no communication with your squad while inside the city
  - you know where they are, but cannot reach them until extraction
  - you are completely alone during each mission

**Objective:**
- Gather scrap
- Follow the signal deeper into the city
- Reach the source of the signal
- Return before the sandstorm arrives

**Social System:**
- You are part of a survivalist group where each member must contribute to survive
- Your value to the group is determined by what you bring back
- Failure to contribute means you have no place in the squad

**Narrative Themes:**
- Isolation and solitude
- Arrival too late to change anything
- Collapse caused by human actions, not nature
- False hope (the signal) leading to emptiness
- Survival without purpose beyond continuation

---

## Core Gameplay Loop
1. Deploy into the city (vehicle drop-off, non-playable)
2. Enter structure at lower accessible level
3. Traverse upward (climbing, vaulting, stacking objects)
4. Encounter robots and environmental hazards
5. Gather scrap
6. Decide when to extract (before storm arrival)
7. Return to vehicle
8. Pay required scrap "tax"
9. Keep excess as profit
10. Upgrade equipment
11. Repeat with deeper/higher access

---

## Core Systems

### 1. Traversal
- First-person movement
- Vaulting system
- Ladders
- Vertical navigation focus
- No advanced parkour system

---

### 2. Physics Interaction
- Objects can be moved and stacked
- Used to create improvised paths
- No snapping / no building system
- Raw physics-based interaction

**Design Intent:**
- Slightly unstable but usable
- Player-created traversal solutions

---

### 3. Combat
- Purpose: Remove blockers (robots), not core focus
- The game is primarily quiet and exploration-focused
- Robot encounters are infrequent but dangerous
- Enemies often occupy or block key traversal paths

**Design Intent:**
- Maintain a mostly peaceful baseline experience
- Create spikes of tension through limited but meaningful encounters
- Force player decision-making (fight, avoid, reroute, or retreat)

**Weapons (planned progression):**
- Melee weapon (metal tool)
- Pistol
- Rifle / shotgun (TBD)

#### Robot Units
- Single robot type encountered throughout the city
- Behavior and threat level change based on condition ("corruption phase")
- Robots do not run - they walk steadily toward the player
- Robots attack when the player is detected

#### Corruption Phases

**1. Broken**
- Missing parts but still functional
- Only melee attacks at melee range

**2. Malfunctioning**
- Unstable and erratic behavior
- Emits distorted or nonsensical audio
- Attacks using rapid, high-intensity molten bolts from a distance

**3. Corrupted**
- Severely degraded structure (rust, holes, environmental damage)
- Extremely dangerous despite instability
- Uses heavy, forceful attacks (throwing metal debris)
- High risk of lethal hits or environmental knockback (falling)

**Design Intent:**
- Maintain minimal enemy variety while increasing threat
- Reflect environmental decay through enemy behavior
- Scale danger without introducing new systems or complexity

---

### 4. Survival Systems

#### 4.1 Air Filtration
- Required to breathe at higher elevations
- Acts as vertical progression gate
- Upgradable via equipment

#### 4.2 Environmental Protection
- Cold increases with altitude due to wind
- Requires suit upgrades
- Acts as additional progression gate

---

### 5. Economy (Scrap System)

#### Scrap Collection
- Primary resource
- Found throughout environment

#### Mission Requirement ("Tax")
- Minimum scrap required per mission
- Increases over time

#### Reward Structure
- Scrap collected up to minimum = consumed (tax)
- Scrap collected beyond minimum = player profit

**Design Intent:**
- Prevent passive play
- Force risk/reward decisions
- Encourage pushing further each run

---

### 6. Time Constraint (Sandstorm System)
- Each mission is limited by an incoming sandstorm (~15–30 minutes)
- The storm acts as the mission timer

**Failure Conditions:**
- Player is caught in the storm > lost
- Extraction vehicle (scrap car) may be buried > cannot return

**Design Intent:**
- Natural, in-world time pressure
- Forces extraction decisions
- Reinforces hostile environment

---

## Progression
- Unlock ability to go higher and deeper via:
  - Filter upgrades
  - Suit upgrades
  - Weapons

**Progression Type:**
- Functional (not skill tree-based)
- Environment-driven gating

---

## Player Motivation
- Reach higher levels of the city
- Maximize profit per run
- Push limits of survival vs reward
- Improve efficiency of traversal and routes

---

## Game Structure
- Mission-based runs
- No open world
- No free exploration outside mission structure

---

## Save System
- No manual saving
- Game autosaves only after successful mission completion (extraction)
- If the player dies or fails to extract, all progress from that mission is lost

**Design Intent:**
- Reinforce risk/reward structure of each run
- Prevent mid-run recovery or save-scumming
- Make each mission self-contained and meaningful

---

## World Persistence & Change
- The city persists between missions - previously explored areas remain mostly unchanged
- Minor environmental changes occur between runs:
  - Some paths may become blocked (sealed by robot repair units)
  - New paths may open (structural collapse, doors opening)
- Changes increase in intensity at higher elevations
- Despite changes, there is always at least one valid path toward the objective

**Design Intent:**
- Maintain player familiarity and learned routes
- Introduce variability without randomness or disorientation
- Encourage adaptation and route re-evaluation each run

---

## Systems NOT Included (Intentional Constraints)
- No drivable vehicles (only start/end sequence)
- No inventory weight system
- No crafting systems
- No complex AI systems
- No story-heavy systems
- No large enemy variety
- No skill trees or RPG systems

---

## Visual Direction
- Industrial / metallic structures
- Rusted, worn materials
- Sand intrusion into architecture
- High vertical density
- Harsh lighting contrast (bright desert vs dark interiors)

---

## Audio Direction
- Wind as dominant element
- Wind intensity increases with elevation
- Strong wind reduces auditory clarity and limits perception
- Metal creaking and structural noise
- Sparse ambient music (non-dominant, situational)
- No combat music - encounters are part of the continuous soundscape
- Robots emit mechanical and threatening audio cues
- Strong emphasis on spatial (3D) audio positioning
- Environmental storytelling through sound

---

## Technical Purpose
This project serves as:
- A real-world validation of Elypso engine
- A systems test environment for:
  - Rendering backends
  - Entity handling at scale
  - Physics interaction
  - Input > simulation > rendering pipeline
