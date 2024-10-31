// Ilie Dumitru

#include <algorithm>
#include <vector>
#include "glm/glm.hpp"

namespace phys
{
	using vec2 = glm::vec2;

	/// Produs scalar
	float dotProduct(vec2 a_u, vec2 a_v)
	{
		return a_u.x * a_v.x + a_u.y * a_v.y;
	}

	const float EPS = (float)1e-4;

	/** Informatii despre corpul respectiv.
	@assert: m_isStatic  <=>  m_invMass == 0
	*/
	struct BodyData
	{
		BodyData() : m_isStatic(true), m_mass(0.f), m_invMass(0.f) {}

		/** Seteaza m_isStatic = a_isStatic, m_mass = a_mass si m_invMass = 1.f / a_mass
		@assert: m_isStatic  <=>  a_mass == 0
		*/
		void setStatic(bool a_isStatic, float a_mass)
		{
			assert(a_mass >= 0.f);
			assert(a_isStatic == (a_mass < EPS));

			if(a_isStatic)
			{
				m_isStatic = true;
				m_mass = m_invMass = 0.f;
			}
			else
			{
				m_isStatic = false;
				m_mass = a_mass;
				m_invMass = 1.f / a_mass;
			}
		}

		/** Intoarce m_isStatic */
		bool isStatic() const
		{
			return m_isStatic;
		}

		/** Intoarce m_mass */
		float getMass() const
		{
			return m_mass;
		}

		/** Intoarce m_invMass */
		float getInvMass() const
		{
			return m_invMass;
		}

	private:
		/// Daca obiectul este static sau nu
		bool m_isStatic;
		/// Masa obiectului
		float m_mass;
		/// 1/m_mass; Ajuta la calculele din spate
		float m_invMass;
	};

	/// Tipurile de forme. Daca se adauga unul nou, se adauga inainte de ShapeTypeCount pentru ca ne trebuie valoarea respectiva sa fie ultima.
	enum ShapeType {SH_CIRCLE, SH_AXIS_RECTANGLE, SH_ROTATIBLE_RECTANGLE, ShapeTypeCount};

	/** Structura de date generica pentru forme */
	struct Shape
	{
		/// Tipul de forma
		ShapeType m_shapeType;
	};

	/** Cerc */
	struct Circle : public Shape
	{
		/// Centru
		vec2 m_center;

		Circle() : m_center(), m_radius() { m_shapeType = SH_CIRCLE; }
		Circle(vec2 a_center, float a_radius) : m_center(a_center), m_radius(a_radius)
		{
			m_shapeType = SH_CIRCLE;
			assert(a_radius > 0);
		}

		/** Seteaza raza */
		void setRadius(float a_radius)
		{
			assert(a_radius >= 0);
			m_radius = a_radius;
		}

		/** Intoarce m_radius */
		float getRadius() const
		{
			return m_radius;
		}

	private:
		/// Raza
		float m_radius;
	};

	/** Dreptunghi cu laturile paralele cu axele de coordonate */
	struct AxisParalelRectangle : public Shape
	{
		/// Centru
		vec2 m_center;

		AxisParalelRectangle() : m_center(), m_halfDims() { m_shapeType = SH_AXIS_RECTANGLE; }
		AxisParalelRectangle(vec2 a_center, vec2 a_halfDims) : m_center(a_center), m_halfDims(a_halfDims) { m_shapeType = SH_AXIS_RECTANGLE; }

		/** Seteaza dimensiunile dreptunghiului */
		void setDimensions(vec2 a_dimensions)
		{
			assert(a_dimensions.x >= 0.f);
			assert(a_dimensions.y >= 0.f);
			m_halfDims = a_dimensions * 0.5f;
		}

		/** Seteaza jumatatea de dimensiuni (dimensiuni / 2 = a_halfDimensions) */
		void setHalfDimensions(vec2 a_halfDimensions)
		{
			assert(a_halfDimensions.x >= 0.f);
			assert(a_halfDimensions.y >= 0.f);
			m_halfDims = a_halfDimensions;
		}

		/** Intoarce dimensiunile dreptunghiului */
		vec2 getDimensions() const
		{
			return m_halfDims * 2.f;
		}

		/** Intoarce jumatatea dimensiunilor dreptunghiului */
		vec2 getHalfDimensions() const
		{
			return m_halfDims;
		}

	private:

		/// Jumatate din dimensiuni
		vec2 m_halfDims;
	};

	/** Dreptunghi rotibil
	@assert: Aceasta forma geometrica TREBUIE sa corespunda unui corp static, care sa nu se astepte la intersectii cu alte dreptunghiuri
	*/
	struct RotatibleRectangle : public Shape
	{
		/// Centru (atat al dreptunghiului cat si a rotatiei)
		vec2 m_center;

		/// Rotatia (in radiani)
		float m_rotation;

		RotatibleRectangle() : m_center(), m_rotation(), m_halfDims() { m_shapeType = SH_ROTATIBLE_RECTANGLE; }
		RotatibleRectangle(vec2 a_center, vec2 a_halfDims, float a_rotation) : m_center(a_center), m_rotation(a_rotation), m_halfDims(a_halfDims) { m_shapeType = SH_ROTATIBLE_RECTANGLE; }

		/** Seteaza dimensiunile dreptunghiului */
		void setDimensions(vec2 a_dimensions)
		{
			assert(a_dimensions.x >= 0.f);
			assert(a_dimensions.y >= 0.f);
			m_halfDims = a_dimensions * 0.5f;
		}

		/** Seteaza jumatatea de dimensiuni (dimensiuni / 2 = a_halfDimensions) */
		void setHalfDimensions(vec2 a_halfDimensions)
		{
			assert(a_halfDimensions.x >= 0.f);
			assert(a_halfDimensions.y >= 0.f);
			m_halfDims = a_halfDimensions;
		}

		/** Intoarce dimensiunile dreptunghiului */
		vec2 getDimensions() const
		{
			return m_halfDims * 2.f;
		}

		/** Intoarce jumatatea dimensiunilor dreptunghiului */
		vec2 getHalfDimensions() const
		{
			return m_halfDims;
		}

	private:
		/// Jumatate din dimensiuni
		vec2 m_halfDims;
	};

	/** Corp = Date despre corp (masa, chestii) + Forma */
	struct Body
	{
		/// Date despre corp
		BodyData m_bodyData;

		/// Viteza
		vec2 m_speed;

		/// Acceleratie
		vec2 m_acceleration;

		Body() : m_bodyData(), m_shape(nullptr), m_speed(), m_acceleration() {}
		~Body()
		{
			delete m_shape;
			m_shape = nullptr;
		}
		Body(const Body& o) = delete;
		Body(Body&& o) = delete;
		Body& operator=(const Body& o) = delete;
		Body& operator=(Body&& o) = delete;

		/** Seteaza forma corpului la un cerc */
		void setShape(Circle a_circle)
		{
			delete m_shape;
			m_shape = new Circle(a_circle);
		}

		/** Seteaza forma corpului la un dreptunghi cu laturile paralele cu axele */
		void setShape(AxisParalelRectangle a_rectangle)
		{
			delete m_shape;
			m_shape = new AxisParalelRectangle(a_rectangle);
		}

		/** Seteaza forma corpului la un dreptunghi rotibil */
		void setShape(RotatibleRectangle a_rectangle)
		{
			delete m_shape;
			m_shape = new RotatibleRectangle(a_rectangle);
		}

		/** Intoarce m_shape
		GRIJA MARE TREBUIE ACORDATA, FORMA POATE FI MODIFICATA IN AFARA FUNCTIEI
		*/
		Shape* getShape()
		{
			return m_shape;
		}

		vec2& getCenter()
		{
			switch(m_shape->m_shapeType)
			{
			case SH_CIRCLE:
				return ((Circle*)m_shape)->m_center;
			case SH_AXIS_RECTANGLE:
				return ((AxisParalelRectangle*)m_shape)->m_center;
			case SH_ROTATIBLE_RECTANGLE:
				return ((RotatibleRectangle*)m_shape)->m_center;
			default:
				// Nu ar trebui sa se poata ajunge aici
				assert(false);
				// Aici nu se ajunge, dar ne trebuie un return peste tot
				return m_speed;
			}
		}

		/** Modifica poizitia si viteza corpului. Folosim integrare newtoniana. */
		void move(const float a_deltaTime)
		{
			if(m_bodyData.isStatic())
				return;

			assert(m_shape);

			vec2& pos = getCenter();
			pos += a_deltaTime * m_speed;
			m_speed += a_deltaTime * m_acceleration;
		}

	private:
		/// Forma
		Shape* m_shape;
	};

	constexpr float ELASTICITY = 0.7f;

	/// Detectie + rezolvare de coliziuni. Am folosit urmatorul tutorial: https://code.tutsplus.com/how-to-create-a-custom-2d-physics-engine-the-basics-and-impulse-resolution--gamedev-6331t
	namespace collision
	{
		/// Checker + rezolvator de coliziuni intre doua cercuri
		void circleToCircleResolver(Body* a_body0, Body* a_body1)
		{
			Circle* A = (Circle*)(a_body0->getShape());
			Circle* B = (Circle*)(a_body1->getShape());

			// Verificare daca se intersecteaza sau nu
			vec2 delta = A->m_center - B->m_center;
			if(delta.x * delta.x + delta.y * delta.y >= (A->getRadius() + B->getRadius()) * (A->getRadius() + B->getRadius()))
				return;

			// Avem intersectie, vrem sa le separam

			// Normala coliziunii
			vec2 normal = B->m_center - A->m_center;
			float normMagn = sqrt(normal.x * normal.x + normal.y * normal.y);
			normal /= normMagn;
			float penetrationDepth = A->getRadius() + B->getRadius() - normMagn;

			// Magnitudinea normalei
			float magnitude = -(1 + ELASTICITY) * (dotProduct(a_body1->m_speed - a_body0->m_speed, normal)) \
							  / (a_body0->m_bodyData.getInvMass() + a_body1->m_bodyData.getInvMass());

			// Impuls
			vec2 impulse = normal * magnitude;
			a_body0->m_acceleration -= impulse * a_body0->m_bodyData.getInvMass();
			a_body1->m_acceleration += impulse * a_body1->m_bodyData.getInvMass();

			// Corectie
			const float percent = 20.f; // A NU SE MODIFICA
			vec2 correction = penetrationDepth / (a_body0->m_bodyData.getInvMass() + a_body1->m_bodyData.getInvMass()) * percent * normal;
			a_body0->m_acceleration -= a_body0->m_bodyData.getInvMass() * correction;
			a_body1->m_acceleration += a_body1->m_bodyData.getInvMass() * correction;
		}

		/// Checker + rezolvator de coliziuni intre un cerc si un dreptunghi
		void circleToAxisResolver(Body* a_body0, Body* a_body1)
		{
			// Presupun ca toate obiectele de tip dreptunghi sunt statice
			assert(a_body1->m_bodyData.isStatic());

			Circle* A = (Circle*)(a_body0->getShape());
			AxisParalelRectangle* B = (AxisParalelRectangle*)(a_body1->getShape());

			// Gasim cel mai apropiat punct din dreptunghi de cerc
			vec2 close;
			close.x = std::max(B->m_center.x - B->getHalfDimensions().x, std::min(B->m_center.x + B->getHalfDimensions().x, A->m_center.x));
			close.y = std::max(B->m_center.y - B->getHalfDimensions().y, std::min(B->m_center.y + B->getHalfDimensions().y, A->m_center.y));

			// Verificare daca se intersecteaza sau nu
			if((close.x - A->m_center.x) * (close.x - A->m_center.x) + (close.y - A->m_center.y) * (close.y - A->m_center.y) >= A->getRadius() * A->getRadius())
				return;

			// Avem intersectie, vrem sa le separam
			vec2 normal;
			float penetrationDepth;

			// Exista doua cazuri, cand centrul cercului este in dreptunghi si cand nu este
			if(std::fabs(close.x - A->m_center.x) < EPS && std::fabs(close.y - A->m_center.y) < EPS)
			{
				float x0, x1, y0, y1;

				x0 = close.x - B->m_center.x + B->getHalfDimensions().x;
				x1 = B->m_center.x + B->getHalfDimensions().x - close.x;
				y0 = close.y - B->m_center.y + B->getHalfDimensions().y;
				y1 = B->m_center.y + B->getHalfDimensions().y - close.y;

				normal.x = normal.y = 0;

				if(std::min(x0, x1) < std::min(y0, y1))
				{
					if(x0 < x1)
					{
						normal.x = -1;
						penetrationDepth = x0;
					}
					else
					{
						normal.x = 1;
						penetrationDepth = x1;
					}
				}
				else
				{
					if(y0 < y1)
					{
						normal.y = -1;
						penetrationDepth = y0;
					}
					else
					{
						normal.y = 1;
						penetrationDepth = y1;
					}
				}


			}
			else
			{
				// Normala coliziunii
				normal = A->m_center - close;
				float len = sqrt(normal.x * normal.x + normal.y * normal.y);
				normal /= len;
				penetrationDepth = A->getRadius() - len;
			}

			// Magnitudinea normalei
			float magnitude = (1 + ELASTICITY) * dotProduct(a_body0->m_speed, normal) * a_body0->m_bodyData.getMass();

			// Impuls
			vec2 impulse = normal * magnitude;
			a_body0->m_acceleration -= impulse * a_body0->m_bodyData.getInvMass();

			// Corectie
			const float percent = 20.f; // A NU SE MODIFICA
			vec2 correction = penetrationDepth * percent * normal;
			a_body0->m_acceleration += correction;
		}

		/// Checker + rezolvator de coliziuni intre un cerc si un dreptunghi rotibil
		void circleToRotatibleResolver(Body* a_body0, Body* a_body1)
		{
			// Presupun ca toate obiectele de tip dreptunghi sunt statice
			assert(!a_body0->m_bodyData.isStatic() && a_body1->m_bodyData.isStatic());

			Circle* A = (Circle*)(a_body0->getShape());
			RotatibleRectangle* B = (RotatibleRectangle*)(a_body1->getShape());

			// Calculez centrul cercului in sistemul de referinta al dreptunghiului
			vec2 movedCenter = A->m_center - B->m_center;
			vec2 rotation = vec2(cos(B->m_rotation), -sin(B->m_rotation));
			vec2 newCenter = vec2(rotation.x * movedCenter.x - rotation.y * movedCenter.y, rotation.y * movedCenter.x + rotation.x * movedCenter.y);

			// Gasim cel mai apropiat punct din dreptunghi de cerc
			vec2 close;
			close.x = std::max(-B->getHalfDimensions().x, std::min(B->getHalfDimensions().x, newCenter.x));
			close.y = std::max(-B->getHalfDimensions().y, std::min(B->getHalfDimensions().y, newCenter.y));

			// Verificare daca se intersecteaza sau nu
			if((close.x - newCenter.x) * (close.x - newCenter.x) + (close.y - newCenter.y) * (close.y - newCenter.y) >= A->getRadius() * A->getRadius())
				return;

			// Avem intersectie, vrem sa le separam
			vec2 normal;
			float penetrationDepth;

			// Exista doua cazuri, cand centrul cercului este in dreptunghi si cand nu este
			if(std::fabs(close.x - newCenter.x) < EPS && std::fabs(close.y - newCenter.y) < EPS)
			{
				float x0, x1, y0, y1;

				x0 = close.x + B->getHalfDimensions().x;
				x1 = B->getHalfDimensions().x - close.x;
				y0 = close.y + B->getHalfDimensions().y;
				y1 = B->getHalfDimensions().y - close.y;

				normal.x = normal.y = 0;

				if(std::min(x0, x1) < std::min(y0, y1))
				{
					if(x0 < x1)
					{
						normal.x = -1;
						penetrationDepth = x0;
					}
					else
					{
						normal.x = 1;
						penetrationDepth = x1;
					}
				}
				else
				{
					if(y0 < y1)
					{
						normal.y = -1;
						penetrationDepth = y0;
					}
					else
					{
						normal.y = 1;
						penetrationDepth = y1;
					}
				}
			}
			else
			{
				// Normala coliziunii
				normal = newCenter - close;
				float len = sqrt(normal.x * normal.x + normal.y * normal.y);
				normal /= len;
				penetrationDepth = A->getRadius() - len;
			}

			// Rotesc inapoi normala
			vec2 oldNormal = normal;
			normal.x =  rotation.x * oldNormal.x + rotation.y * oldNormal.y;
			normal.y = -rotation.y * oldNormal.x + rotation.x * oldNormal.y;

			// Magnitudinea normalei
			float magnitude = (1 + ELASTICITY) * dotProduct(a_body0->m_speed, normal);

			// Impuls
			vec2 impulse = normal * magnitude;
			a_body0->m_acceleration -= impulse;

			// Corectie
			const float percent = 20.f; // A NU SE MODIFICA
			vec2 correction = penetrationDepth * percent * normal;
			a_body0->m_acceleration += correction;
		}

		/// Checker + rezolvator de coliziuni intre un dreptunghi si un cerc
		void axisToCircleResolver(Body* a_body0, Body* a_body1) {circleToAxisResolver(a_body1, a_body0);}

		/// Checker + rezolvator de coliziuni intre un dreptunghi rotibil si un cerc
		void rotatibleToCircleResolver(Body* a_body0, Body* a_body1) {circleToRotatibleResolver(a_body1, a_body0);}

		/// Functie care nu face nimic. Exista pentru a nu avea cateva instructiuni de decizie in plus in program
		void nullResolver(Body*, Body*) {}

		using resolver = void (*)(Body*, Body*);

		resolver resolvers[ShapeTypeCount][ShapeTypeCount] = {{circleToCircleResolver, circleToAxisResolver, circleToRotatibleResolver},
															  {axisToCircleResolver, nullResolver, nullResolver},
															  {rotatibleToCircleResolver, nullResolver, nullResolver}};

		/// Rezolva coliziunea dintre cele doua corpuri.
		void collisionResolver(Body* a_body0, Body* a_body1)
		{
			// Daca ambele sunt statice atunci nu facem nimic.
			if(a_body0->m_bodyData.isStatic() && a_body1->m_bodyData.isStatic())
				return;

			resolvers[a_body0->getShape()->m_shapeType][a_body1->getShape()->m_shapeType](a_body0, a_body1);
		}

		/// Primeste doua cercuri diferite si returneaza true daca si numai daca cele doua se intersecteaza
		bool checkCollision(Circle* a_circle, Circle* a_collider)
		{
			assert(a_circle && a_collider && a_circle != a_collider);

			// Distanta la patrat e mai mica (putem adauga sau egala dar fiind pe floats e cam de geaba) decat suma razelor la patrat
			return (a_circle->m_center.x - a_collider->m_center.x) * (a_circle->m_center.x - a_collider->m_center.x)
			     + (a_circle->m_center.y - a_collider->m_center.y) * (a_circle->m_center.y - a_collider->m_center.y)
			     < (a_circle->getRadius() + a_circle->getRadius()) * (a_circle->getRadius() + a_circle->getRadius());
		}
	}

	/// Informatii despre lumea din sistemul de fizica + functii relevante
	struct World
	{
		// Corpurile din lume
		std::vector<Body*> m_bodies;

		World()
		{
			// Hack pentru pointeri.
			m_bodies.reserve(100);
		}
		~World()
		{
			for(Body* body : m_bodies)
				delete body;
		}
		World(const World& o) = delete;
		World(World&& o) = delete;
		World& operator=(const World& o) = delete;
		World& operator=(World&& o) = delete;

		/// Creeaza un corp in lume si intoarce un pointer la acesta. Informatiile referitoare la corp trebuie modificate extern
		Body* makeBody()
		{
			m_bodies.push_back(new Body());
			return m_bodies.back();
		}

		/// Avanseaza lumea cu a_deltaTime secunde.
		void tick(const float a_deltaTime, const int runs)
		{
			int i, j, N = (int)m_bodies.size(), run;

			for(run = 0;run < runs;++run)
			{
				for(i = 0;i < N;++i)
					m_bodies[i]->m_acceleration = vec2();

				// Coliziuni
				for(i = 1;i < N;++i)
					for(j = 0;j < i;++j)
						collision::collisionResolver(m_bodies[i], m_bodies[j]);

				for(i = 0;i < N;++i)
					m_bodies[i]->move(a_deltaTime);
			}
		}
	};
}