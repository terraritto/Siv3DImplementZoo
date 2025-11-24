#pragma once
#include <Siv3D.hpp>

void DiskTraceSample()
{
	class Object;
	constexpr double INFINITY_VALUE = 1e10;
	constexpr double EPSILON_VALUE = 1e-8;
	constexpr double BIAS_VALUE = 1e-4;

	class Interval
	{
	public:
		double m_min, m_max;

		Interval(double minValue, double maxValue) : m_min(minValue), m_max(maxValue) {}

		bool Surrounds(double v) const
		{
			return m_min < v && v < m_max;
		}

		bool Contains(double v) const
		{
			return m_min <= v && v <= m_max;
		}
	};

	class Light
	{
	public:
		Light(Vec3 position, double intensity) : m_position(position), m_intensity(intensity) {}

		Vec3 m_position;
		double m_intensity;
	};

	enum class MaterialType { DIFFUSE };

	class Material
	{
	public:
		MaterialType m_type = MaterialType::DIFFUSE;
		ColorF m_diffuseColor = Palette::Black;
		double kd = 0.8, ks = 0.2;
		double ksExp = 25.0;
		double ior = 1.3;
		Vec2 uv;

		virtual ColorF diffuse() const { return m_diffuseColor; }
	};

	class Checker : public Material
	{
	public:
		ColorF diffuse() const
		{
			double scale = 5.0;
			double pattern = (Math::Fmod(uv.x * scale, 1.0) > 0.5) ^ (Math::Fmod(uv.y * scale, 1.0) > 0.5);
			return ColorF{ Math::Lerp(Vec3(0.815, 0.235, 0.031), Vec3(0.937, 0.937, 0.231), pattern) };
		}
	};

	class Record
	{
	public:
		double m_t = INFINITY_VALUE;
		Vec3 m_p;
		Vec3 m_normal;
		std::weak_ptr<Material> m_material;
	};

	class Object
	{
	public:
		virtual bool Intersect(const Ray& ray, const Interval& interval, Record& record) = 0;

	public:
		std::shared_ptr<Material> m_material;
	};

	class Disk : public Object
	{
	public:
		Disk(const Vec3& center, const Vec3& normal, const double radius)
			: m_center(center)
			, m_normal(normal)
			, m_radius(radius)
		{
			m_normal.normalize();
		}

		bool Intersect(const Ray& ray, const Interval& interval, Record& record) override
		{
			double t = Dot(m_center - ray.origin.xyz(), m_normal) / Dot(ray.direction, m_normal);

			if (t <= EPSILON_VALUE) { return false; }

			Vec3 p = ray.origin.xyz() + t * ray.direction;
			if ((p - m_center).lengthSq() < m_radius * m_radius)
			{
				record.m_t = t;
				record.m_p = p;
				record.m_normal = m_normal;
				record.m_material = this->m_material;
				return true;
			}

			return false;
		}

	protected:
		Vec3 m_center;
		Vec3 m_normal;
		double m_radius;
	};

	class RenderResource
	{
	public:
		Image m_image = Image{ Scene::Size() };
		ColorF m_background;
		double m_fov;
		int m_maxDepth;
	};

	auto Hit = [](const Ray& ray, const Array<std::shared_ptr<Object>>& objects, Record& record)
		{
			double tNear = INFINITY_VALUE;
			bool isHit = false;
			for (const auto& object : objects)
			{
				Interval interval{ 0, INFINITY_VALUE };
				Record temp;
				if (object->Intersect(ray, interval, temp))
				{
					if (temp.m_t < tNear)
					{
						isHit = true;
						tNear = temp.m_t;
						record = temp;
					}
				}
			}

			return isHit;
		};

	auto Trace = [&](auto self, const Ray& ray, const Array<std::shared_ptr<Object>>& objects, const Array<std::shared_ptr<Light>>& lights, RenderResource& resource, int depth)
		{
			auto Reflect = [](const Vec3& i, const Vec3& n)
				{
					return i - 2 * Dot(i, n) * n;
				};

			// 再帰の打ち切り
			if (depth > resource.m_maxDepth) { return resource.m_background; }

			ColorF result = ColorF{ 0,0,0 };

			Record record;
			if (Hit(ray, objects, record))
			{
				auto mat = record.m_material.lock();
				switch (mat->m_type)
				{
				case MaterialType::DIFFUSE:
					// Phong
				{
					for (const auto& light : lights)
					{
						// Lightの方向と距離を用意
						Vec3 lightDir = light->m_position - record.m_p;
						double lightDistance2 = Dot(lightDir, lightDir);
						lightDir.normalize();

						// Cos項を計算
						float LoN = Max(0.0, Dot(lightDir, record.m_normal));

						// Shadow Rayを生成
						Vec3 shadowOrigin = Dot(ray.direction.xyz(), record.m_normal) < 0.0 ?
							record.m_p + record.m_normal * BIAS_VALUE :
							record.m_p - record.m_normal * BIAS_VALUE;
						Ray shadowRay{ shadowOrigin, lightDir };

						// Shadow Rayを飛ばす
						Record shadowRecord;
						bool isNotEvaluate = Hit(shadowRay, objects, shadowRecord); // そもそもHitしてる？
						Vec3 sd = (shadowOrigin - shadowRecord.m_p);
						isNotEvaluate &= Dot(sd, sd) < lightDistance2; // Lightより手前で当たってる？
						if (isNotEvaluate == false)
						{
							// 計算するべき
							result += light->m_intensity * LoN * mat->kd * mat->diffuse(); // diffuse
							Vec3 reflect = Reflect(-lightDir, ray.direction.xyz());
							result += ColorF{ Pow(Max(0.0, Dot(reflect, ray.direction)), mat->ksExp) * light->m_intensity * mat->ks };
						}
					}
				}
				break;
				}
			}
			else
			{
				// 何も当たってない場合は背景色
				result = resource.m_background;
			}

			return result;
		};

	// 実際の描画
	auto Render = [Trace](const Array<std::shared_ptr<Object>>& objects, const Array<std::shared_ptr<Light>>& lights, RenderResource& resource)
		{
			const int width = resource.m_image.width();
			const int height = resource.m_image.height();

			double theta = ToRadians(resource.m_fov);
			double hh = tan(theta * 0.5);
			double aspect = width / static_cast<double>(height);
			Vec3 Origin = Vec3::Zero();

			for (int h = 0; h < height; h++)
			{
				for (int w = 0; w < width; w++)
				{
					double x = (2.0 * (w + 0.5) / static_cast<double>(width) - 1.0) * aspect * hh;
					double y = (1.0 - 2.0 * (h + 0.5) / static_cast<double>(height)) * hh;
					Ray ray{ Origin, Vec3{x,y,-1} };
					resource.m_image[h][w] = Trace(Trace, ray, objects, lights, resource, 0);
				}
			}
		};

	// Objectの構築
	Array<std::shared_ptr<Object>> objects;
	{
		// Quad
		std::shared_ptr<Object> obj = std::make_shared<Disk>(Vec3{ 0.0, 0.5, -3 }, Vec3{ 0.9, 0.0, 0.5 }, 1.5);
		std::shared_ptr<Material> mat = std::make_shared<Material>();
		mat->m_diffuseColor = Palette::Yellow;
		obj->m_material = mat;
		objects.emplace_back(std::move(obj));
	}


	// Lightの構築
	Array<std::shared_ptr<Light>> lights;
	lights.emplace_back(std::make_shared<Light>(Vec3{ 0,0,5 }, 10));
	//lights.emplace_back(std::make_shared<Light>(Vec3{ 30,50,-12 }, 1.0));

	RenderResource resource;
	resource.m_background = ColorF(0.235294, 0.67451, 0.843137, 1.0);
	resource.m_fov = 90.0;
	resource.m_maxDepth = 5;

	// 描画！
	Render(objects, lights, resource);

	DynamicTexture texture{ resource.m_image };
	texture.fill(resource.m_image);

	while (System::Update())
	{
		texture.draw();
	}
}
