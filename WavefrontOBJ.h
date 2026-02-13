/*
  WavefrontOBJ.h

  Wavefront OBJ Parser - C++ 98 Class for Parsing Wavefront obj file

  A header-only C++ code solution for parsing Wavefront OBJ files, with no
  external dependencies other than the Standard Template Library (STL).

  The solution is self-contained in a single header file, making it easy to
  integrate into existing projects without the need for additional libraries
  or dependencies.

  This solution has been tested and verified to be compatible with C++ 98 
  environments. It utilizes features and syntax available within the C++ 98 
  standard, ensuring its functionality in older C++ environments.

  This software is released under the terms of the
  GNU General Public License v3.0. Details and terms of this
  license can be found at: https://www.gnu.org/licenses/gpl-3.0.html
  For those who require the freedom to operate without the
  constraints of the GPL, a commercial license can be obtaining by
  contacting the author at stefan.johnsen@outlook.com
 */

#pragma once

#ifndef WAVEFRONT_OBJ
#define WAVEFRONT_OBJ

#include <algorithm>
#include <stdio.h>
#include <string>
#include <vector>
#include <tuple>
#include <cmath>
#include <map>
#include <sys/stat.h>

namespace obj
{
	template <typename T>
	struct List
	{
		size_t size() const;

		bool empty() const;

		void clear();

		void insert(const std::vector<T>& list);

		void insert(typename std::vector<T>::iterator begin, typename std::vector<T>::iterator end);

		std::vector<T>   v;
		std::vector<int> s;
	};

	struct Vertex : List<float> { };

	struct Texture : List<float> { };

	struct Normal : List<float> { };

	struct Face
	{
		void clear();

		List<int> vertex;
		List<int> texture;
		List<int> normal;
	};

	struct Line
	{
		void clear();

		List<int> vertex;
		List<int> texture;
	};

	struct Point
	{
		void clear();

		List<int> vertex;
	};

	struct Material;
	struct Information;

	class Load
	{
	public:

		explicit Load(bool triangulate = false);

		~Load();

		bool load(const std::string& path);

		std::string mtllib() const;

		std::vector<Material>& usemtl();

		Vertex  vertex;  //Geometric vertices
		Texture texture; //Texture vertices
		Normal  normal;  //Normal vertices
		Face    face;    //Indices face
		Line    line;    //Indices line
		Point   point;   //Indices point

		void clear();

	private:

		bool open(const std::string& path);

		bool load(char** document, size_t rows);

		void close();

		FILE*						file;
		std::string                 path;
		std::string                 materialFile;
		std::vector<Material>		materialFace;
		std::vector<Information>	information;
		bool                        triangulate;
	};

	//-------------------------------------------------------------------------------------------------------

	struct Material
	{
		Material(const std::string& name, size_t face) : name(name), face(face) {}

		Material& operator=(const Material&);

		void clear();

		std::string name;
		size_t face;
	};

	struct Information
	{
		Information(char ch, const std::string& information, size_t face) : ch(ch), information(information), face(face) {}

		void clear();

		char ch;
		std::string information;
		size_t face;
	};

	//-------------------------------------------------------------------------------------------------------

	char* trim(char*);

	bool parse(const char*, Vertex&);

	bool parse(const char*, Normal&);

	bool parse(const char*, Texture&);

	bool parse(const char*, Point&, size_t);

	bool parse(const char*, Line&, size_t);

	bool parse(const char*, Face&, size_t, bool);

	bool parse(char*, std::string&);

	bool parse(char*, std::vector<Material>&, size_t);

	bool parse(char*, std::vector<Information>&, size_t);

	size_t createMemory(FILE*, char*&, size_t&);

	bool createDocument(char*&, size_t, char**&, size_t);

	void insert_indices(List<int>&, const std::vector<int>&, bool);

	void triangulate_indices(List<int>&, const std::vector<int>&);

	//-------------------------------------------------------------------------------------------------------

	inline Load::Load(const bool triangulate) : file(nullptr), triangulate(triangulate) { }

	inline Load::~Load() { close(); }

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable:4996) // fopen is deprecated for MSVC, but its fast and OS independent
#endif

	inline bool Load::open(const std::string& open_path)
	{
		close();

		path = open_path;

		file = fopen(path.c_str(), "rb");

		if (file) return true;

		printf("Impossible to open obj the file !\n");

		return false;
	}

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

	inline void Load::clear()
	{
		vertex.clear();
		texture.clear();
		normal.clear();
		face.clear();
		line.clear();
		point.clear();

		information.clear();
		materialFace.clear();
		materialFile.clear();
	}

	inline void Load::close()
	{
		if (!file) return;

		fclose(file);

		file = nullptr;
	}

	inline bool Load::load(const std::string& path)
	{
		close();

		clear();

		if (!open(path))
			return false;

		struct stat st;
		stat(path.c_str(), &st);
		size_t size = st.st_size;

		if (size == 0)
			return false;

		char* memory = nullptr;

		const size_t rows = createMemory(file, memory, size);

		if (rows == 0)
			return false;

		if (memory == nullptr)
			return false;

		char** document = nullptr;

		const bool create = createDocument(memory, size, document, rows);

		if (!create)
			return false;

		if (document == nullptr)
			return false;

		const bool res = load(document, rows);

		delete[] memory;

		delete[] document;

		close();

		return res;
	}

	inline bool Load::load(char** document, const size_t rows)
	{
		if (document == nullptr) return false;

		char* line;

		std::string text;

		bool proceed(true);

		for (size_t row = 0; row < rows; row++)
		{
			line = trim(document[row]);

			if (*line == 'f' && *(line + 1) == ' ')
				proceed = parse(line + 2, face, vertex.size(), triangulate);
			else if (*line == 'v' && *(line + 1) == ' ')
				proceed = parse(line + 2, vertex);
			else if (*line == 'v' && *(line + 1) == 'n')
				proceed = parse(line + 3, normal);
			else if (*line == 'v' && *(line + 1) == 't')
				proceed = parse(line + 3, texture);
			else if (*line == 'u')
				proceed = parse(line, materialFace, face.vertex.size());
			else if ((*line == '#' || *line == 'o' || *line == 'g' || *line == 's') && *(line + 1) == ' ')
				proceed = parse(line, information, face.vertex.size());
			else if (*line == 'l' && *(line + 1) == ' ')
				proceed = parse(line + 2, this->line, vertex.size());
			else if (*line == 'p' && *(line + 1) == ' ')
				proceed = parse(line + 2, point, vertex.size());
			else if (*line == 'm')
				proceed = parse(line, materialFile);

			if (proceed == false) return false;
		}

		return true;
	}

	inline std::string setExtension(std::string file, std::string ext)
	{
		const size_t find = file.rfind('.');

		if (find == std::string::npos)
			return std::string();

		file.erase(find + 1);

		return file + ext;
	}

	inline std::string Load::mtllib() const
	{
		std::string mtl(path);

		if (materialFile.empty())
			return setExtension(mtl, "mtl");

		size_t find = mtl.rfind('\\');

		if (find == std::string::npos)
			find = mtl.rfind('/');

		if (find == std::string::npos)
			return setExtension(mtl, "mtl");

		mtl.erase(find + 1);

		mtl = mtl + materialFile;

		return mtl;
	}

	inline std::vector<Material>& Load::usemtl()
	{
		return materialFace;
	}

	//-------------------------------------------------------------------------------------------------------

	template <typename T>
	size_t List<T>::size() const { return s.size(); }

	template <typename T>
	bool List<T>::empty() const { return s.empty(); }

	template <typename T>
	void List<T>::insert(const std::vector<T>& list)
	{
		v.insert(v.end(), list.begin(), list.end());
		s.push_back(static_cast<int>(list.size()));
	}

	template <typename T>
	void List<T>::insert(typename std::vector<T>::iterator begin, typename std::vector<T>::iterator end)
	{
		v.insert(v.end(), begin, end);
		s.push_back(static_cast<int>(end - begin));
	}

	template <typename T>
	void List<T>::clear()
	{
		v.clear();
		s.clear();
	}

	//-------------------------------------------------------------------------------------------------------

	inline void Face::clear()
	{
		vertex.clear();
		texture.clear();
		normal.clear();
	}

	inline void Line::clear()
	{
		vertex.clear();
		texture.clear();
	}

	inline void Point::clear()
	{
		vertex.clear();
	}

	inline void Material::clear()
	{
		face = 0;
	}
	
	inline void Information::clear()
	{
		face = 0;
	}

	//-------------------------------------------------------------------------------------------------------

	inline size_t createMemory(FILE* file, char*& memory, size_t& size)
	{
		if (file == nullptr || size == 0)
			return 0;

		memory = new char[size + 1];

		if (memory == nullptr)
			return 0;

		size = fread(memory, sizeof(char), size, file);

		size_t rows(0);

		for (size_t i = 0; i < size; i++)
		{
			if (memory[i] == '\n')
			{
				rows++;

				memory[i] = '\0';
			}
		}

		rows++;

		memory[size] = '\0'; //EOF

		if (rows == 0)
		{
			delete[] memory;

			memory = nullptr;
		}

		return memory ? rows : 0;
	}

	inline bool createDocument(char*& memory, const size_t size, char**& document, const size_t rows)
	{
		if (size == 0 || rows == 0)
			return false;

		document = new char*[rows];

		if (document == nullptr)
			return 0;

		document[0] = &memory[0];

		size_t row(1);

		for (size_t i = 1; i < size; i++)
		{
			if (memory[i] == '\0')
			{
				document[row] = &memory[i + 1]; //memory[size] = EOF (see above)

				row++;
			}
		}

		if (row != rows)
		{
			delete[] document;

			document = nullptr;
		}

		return document ? true : false;
	}

	//-------------------------------------------------------------------------------------------------------

	inline bool strtoi(const char* text, int& i, const char*& end)
	{
		static int v;

		static const char* p;

		static bool negative;

		p = text;

		negative = false;

		while (*p == ' ') p++;

		if (*p == '-')
		{
			negative = true;

			p++;
		}
		else if (*p == '+')
			p++;

		v = 0;

		while (*p >= '0' && *p <= '9')
		{
			v = (v * 10) + (*p - '0');

			p++;
		}

		end = p;

		i = negative ? -v : v;

		return text == end ? false : true;
	}

	inline bool strtof(const char* text, float& d, const char*& end)
	{
		static float v;

		static const char* p;

		static int exponent;

		static float factor;

		static bool negExp;

		static bool negative;

		p = text;

		negative = false;

		while (*p == ' ') ++p;

		if (*p == '-')
		{
			negative = true;

			p++;
		}
		else if (*p == '+')
			p++;

		v = 0.0;

		while (*p >= '0' && *p <= '9')
		{
			v = v * 10.0f + (*p - '0');

			p++;
		}

		if (*p == '.')
		{
			p++;

			factor = 0.1f;

			while (*p >= '0' && *p <= '9')
			{
				v += (*p - '0') * factor;

				factor *= 0.1f;

				p++;
			}
		}

		if (*p == 'e' || *p == 'E')
		{
			++p;

			exponent = 0;

			negExp = false;

			if (*p == '-')
			{
				negExp = true;

				p++;
			}
			else if (*p == '+')
			{
				p++;
			}

			while (*p >= '0' && *p <= '9')
			{
				exponent = exponent * 10 + (*p - '0');

				p++;
			}

			v *= (float)pow(10.0, negExp ? -exponent : exponent);
		}

		end = p;

		d = negative ? -v : v;

		return text == end ? false : true;
	}

	//-------------------------------------------------------------------------------------------------------

	inline bool isspace(const char& c)
	{
		return c == ' ' || c == '\t' || c == '\v';
	}

	inline char* trim(char* p)
	{
		static char* e;

		if (p == nullptr) return nullptr;

		while (std::isspace(*p) && *p != '\0') p++;

		e = p;

		while (*e != '\0') e++;

		if (e != p) e--;

		while (std::isspace(*e) && e != p) e--;

		if (e != p) e++;

		*e = '\0';

		return p;
	}

	//-------------------------------------------------------------------------------------------------------

	inline bool parse(char* line, std::string& text)
	{
		line++; // 'm'

		if (*line++ != 't') return false;
		if (*line++ != 'l') return false;
		if (*line++ != 'l') return false;
		if (*line++ != 'i') return false;
		if (*line++ != 'b') return false;

		text = trim(line);

		return true;
	}

	inline bool parse(char* line, std::vector<Material>& list, const size_t face)
	{
		line++; // 'u'

		if (*line++ != 's') return false;
		if (*line++ != 'e') return false;
		if (*line++ != 'm') return false;
		if (*line++ != 't') return false;
		if (*line++ != 'l') return false;

		list.push_back(Material(trim(line), face));

		return true;
	}

	inline bool parse(char* line, std::vector<Information>& information, const size_t face)
	{
		information.push_back(Information(*line, trim(line + 1), face));

		return true;
	}

	inline bool parse(const char* line, Vertex& item)
	{
		static std::vector<float> vertex(6);

		if (!strtof(line, vertex[0], line))
			return false;

		if (!strtof(line, vertex[1], line))
			return false;

		if (!strtof(line, vertex[2], line))
			return false;

		if (!strtof(line, vertex[3], line))
		{
			item.insert(vertex.begin(), vertex.begin() + 3); //x, y, z

			return true;
		}

		if (!strtof(line, vertex[4], line))
		{
			item.insert(vertex.begin(), vertex.begin() + 4); //x, y, z, w

			return true;
		}

		if (!strtof(line, vertex[5], line))
			return false;

		item.insert(vertex); //x, y, z, r, g, b

		return true;
	}

	inline bool parse(const char* line, Normal& item)
	{
		static std::vector<float> normal(3);

		if (!strtof(line, normal[0], line))
			return false;

		if (!strtof(line, normal[1], line))
			return false;

		if (!strtof(line, normal[2], line))
			return false;

		item.insert(normal);

		return true;
	}

	inline bool parse(const char* line, Texture& item)
	{
		static std::vector<float> texture(3);

		if (!strtof(line, texture[0], line))
			return false;

		if (!strtof(line, texture[1], line))
		{
			item.insert(texture.begin(), texture.begin() + 1);

			return true;
		}

		if (!strtof(line, texture[2], line))
		{
			item.insert(texture.begin(), texture.begin() + 2);

			return true;
		}

		item.insert(texture);

		return true;
	}

	inline bool iseol(const char& c)
	{
		return c == '\r' || c == '\n' || c == '\0';
	}

	inline bool parse(const char* line, Point& item, const size_t pointSize)
	{
		static int i, v;

		static std::vector<int> vertex;

		vertex.clear();

		v = static_cast<int>(pointSize);

		while (!iseol(*line))
		{
			if (!strtoi(line, i, line))
				return false;

			i = i > 0 ? i - 1 : i + v;

			vertex.push_back(i);

			while (isspace(*line))
				line++;
		}

		item.vertex.insert(vertex);

		return true;
	}

	inline bool parse(const char* line, Line& item, const size_t pointSize)
	{
		static int i, v;

		static std::vector<int> vertex;
		static std::vector<int> texture;

		vertex.clear();
		texture.clear();

		v = static_cast<int>(pointSize);

		while (!iseol(*line))
		{
			if (!strtoi(line, i, line))
				return false;

			i = i > 0 ? i - 1 : i + v;

			vertex.push_back(i);

			if (*line == '/')
			{
				line++;

				if (!strtoi(line, i, line))
					return false;

				i = i > 0 ? i - 1 : i + v;

				texture.push_back(i);
			}

			while (isspace(*line))
				line++;
		}

		item.vertex.insert(vertex);
		item.texture.insert(texture);

		return true;
	}

	inline bool parse(const char* line, Face& item, const size_t pointSize, bool triangulate)
	{
		static int i, v;

		static std::vector<int> vertex;
		static std::vector<int> texture;
		static std::vector<int> normal;

		vertex.clear();
		texture.clear();
		normal.clear();

		v = static_cast<int>(pointSize);

		while (!iseol(*line))
		{
			if (!strtoi(line, i, line))
				return false;

			i = i > 0 ? i - 1 : i + v;

			vertex.push_back(i);

			if (*line == '/')
			{
				line++;

				if (*line != '/')
				{
					if (!strtoi(line, i, line))
						return false;

					i = i > 0 ? i - 1 : i + v;

					texture.push_back(i);
				}

				if (*line == '/')
				{
					line++;

					if (!strtoi(line, i, line))
						return false;

					i = i > 0 ? i - 1 : i + v;

					normal.push_back(i);
				}
			}

			while (isspace(*line))
				line++;
		}

		insert_indices(item.vertex, vertex, triangulate);
		insert_indices(item.texture, texture, triangulate);
		insert_indices(item.normal, normal, triangulate);

		return true;
	}

	inline void insert_indices(List<int>& list, const std::vector<int>& indices, bool triangulate)
	{
		if (triangulate && indices.size() > 3)
			triangulate_indices(list, indices);
		else
			list.insert(indices);
	}

	inline void triangulate_indices(List<int>& list, const std::vector<int>& indices)
	{
		static size_t size, index;

		size = indices.size();

		static std::vector<int> triangle;

		for (index = 0; index < size - 2; index++)
		{
			triangle.clear();

			triangle.push_back(indices[index + 1]);
			triangle.push_back(indices[index + 2]);
			triangle.push_back(indices[0]);

			list.insert(triangle);
		}
	}

	// <-------- End of WavefrontOBJ.h 

	//-------------------------------------------------------------------------------------------------------
	// Additional functions to simplify the connection between each face and material/color (Kd)
	//-------------------------------------------------------------------------------------------------------
	
	template <typename LoadMTL>
	size_t connectFaceMaterial(Load& loadOBJ, LoadMTL& loadMTL, std::vector<int>& connect)
	{
		size_t lastFace;

		connect.clear();

		auto& materials = loadMTL.materials();

		if (materials.size() == 0) return 0;

		if (loadOBJ.face.vertex.empty()) return 0;

		std::map<std::string, int> materialNameIndex;

		for (size_t index = 0; index < materials.size(); index++)
		{
			std::string material = materials[index].name;

			materialNameIndex[material] = static_cast<int>(index);
		}

		const std::vector<Material>& materialFace = loadOBJ.usemtl();

		for (size_t index = 0; index < materialFace.size(); index++)
		{
			const Material& item = materialFace[index];

			const std::string& material = item.name;
			const size_t& fromFace = item.face;

			if (index == materialFace.size() - 1)
				lastFace = loadOBJ.face.vertex.size();
			else
			{
				const Material& next = materialFace[index + 1];

				lastFace = next.face;
			}

			int materialIndex(-1);

			if (materialNameIndex.find(material) != materialNameIndex.end())
				materialIndex = materialNameIndex[material];

			std::vector<int> block(lastFace - fromFace, materialIndex);

			connect.insert(connect.end(), block.begin(), block.end());
		}

		if (connect.size() != loadOBJ.face.vertex.size())
			connect.clear();

		return connect.size();
	}

	template <typename LoadMTL, class T>
	size_t loadFaceColor(Load& loadOBJ, LoadMTL& loadMTL, std::vector< std::vector<T> >& color, const bool alpha = false)
	{
		T r = 0;
		T g = 0;
		T b = 1;
		T a = 1;

		color.clear();

		std::vector<int> faceMaterial;

		auto& materialList = loadMTL.materials();

		if (connectFaceMaterial(loadOBJ, loadMTL, faceMaterial) == 0)
		{
			std::vector<T> defaultColor;

			if (!materialList.empty())
			{
				const auto& material = materialList[0];

				const auto& Kd = material.Kd.color;

				r = static_cast<T>(Kd.r);
				g = static_cast<T>(Kd.g);
				b = static_cast<T>(Kd.b);

				if (alpha)
					a = static_cast<T>(material.d.d);
			}

			defaultColor.push_back(r);
			defaultColor.push_back(g);
			defaultColor.push_back(b);

			if (alpha)
				defaultColor.push_back(a);

			color.resize(loadOBJ.face.vertex.size(), defaultColor);

			return color.size();
		}

		color.resize(faceMaterial.size());

		int materialIndex(-1);

		for (size_t face = 0; face < color.size(); face++)
		{
			if (materialIndex != faceMaterial[face])
			{
				if (faceMaterial[face] != -1)
				{
					materialIndex = faceMaterial[face];

					const auto& material = materialList[materialIndex];

					const auto& Kd = material.Kd.color;

					r = static_cast<T>(Kd.r);
					g = static_cast<T>(Kd.g);
					b = static_cast<T>(Kd.b);

					if (alpha)
						a = static_cast<T>(material.d.d);
				}
			}

			color[face].push_back(r);
			color[face].push_back(g);
			color[face].push_back(b);
			
			if (alpha)
				color[face].push_back(a);
		}

		return color.size();
	}

	template <typename LoadMTL, class T>
	size_t Copy(Load& loadOBJ, LoadMTL& loadMTL, std::vector< std::vector<T> >& color, const bool alpha = false)
	{
		return loadFaceColor(loadOBJ, loadMTL, color, alpha);
	}

	template <typename LoadMTL, class T>
	size_t Copy(Load& loadOBJ, LoadMTL& loadMTL, std::vector<T>& color, const bool alpha = false)
	{
		std::vector< std::vector<T> > temp;

		if( loadFaceColor(loadOBJ, loadMTL, temp, alpha) == 0 )
			return 0;

		for(size_t n=0; n<temp.size(); n++)
		{
			std::vector<T>& item = temp[n];

			color.insert(color.end(), item.begin(), item.end());
		}

		return color.size();
	}

	//-------------------------------------------------------------------------------------------------------
	// Additional functions for easier copying or moving of lists from WavefrontOBJ to your working lists
	//-------------------------------------------------------------------------------------------------------

	enum VertexFormat
	{
		xyz = 3,
		xyzw = 4,
		xyzrgb = 6
	};

	inline VertexFormat vertexFormat(const size_t& size)
	{
		switch (size)
		{
		case 3: return xyz;
		case 4: return xyzw;
		case 6: return xyzrgb;
		}

		return xyz;
	}

	inline VertexFormat format(const Vertex& vertex, bool& varies)
	{
		varies = false;

		if (vertex.empty()) return xyz;

		const VertexFormat format = vertexFormat(vertex.s.front());

		for (std::vector<int>::const_iterator it = vertex.s.begin(); it != vertex.s.end(); it++)
		{
			const int size = *it;

			if (vertexFormat(size) != format)
			{
				varies = true;

				return xyz;
			}
		}

		return format;
	}
	
	bool move(Vertex& source, std::vector<float>& target, const VertexFormat format = xyz)
	{
		bool varies(false);

		if (source.empty()) return true;

		if (obj::format(source, varies) == format && !varies)
		{
			target = std::move(source.v);

			source.clear();

			return true;
		}

		return false;
	}

	size_t copy(Vertex& source, std::vector<float>& target, const VertexFormat format = xyz)
	{
		if (move(source, target, format))
			return target.size();

		std::vector<float>::const_iterator vertex = source.v.begin();

		for (std::vector<int>::const_iterator it = source.s.begin(); it != source.s.end(); it++)
		{
			const int size = *it;

			if (format == size)
				target.insert(target.end(), vertex, vertex + size);
			else
			{
				std::vector<float> item;

				item.push_back(size > 0 ? *(vertex + 0) : 0.0f);
				item.push_back(size > 1 ? *(vertex + 1) : 0.0f);
				item.push_back(size > 2 ? *(vertex + 2) : 0.0f);

				if (format == xyzw)
				{
					item.push_back(0.0f);
				}

				if (format == xyzrgb)
				{
					item.push_back(0.0f);
					item.push_back(0.0f);
					item.push_back(0.0f);
				}

				target.insert(target.end(), item.begin(), item.end());
			}

			vertex += size;
		}

		return target.size();
	}

	template <typename T>
	size_t copy(const Vertex& source, std::vector<T>& target, const VertexFormat format = xyz)
	{
		std::vector<float>::const_iterator vertex = source.v.begin();

		for (std::vector<int>::const_iterator it = source.s.begin(); it != source.s.end(); it++)
		{
			const int size = *it;

			std::vector<T> item;

			item.push_back((T)(size > 0 ? *(vertex + 0) : 0));
			item.push_back((T)(size > 1 ? *(vertex + 1) : 0));
			item.push_back((T)(size > 2 ? *(vertex + 2) : 0));

			if (format == xyzw)
			{
				item.push_back((T)(0));
			}

			if (format == xyzrgb)
			{
				item.push_back((T)(0));
				item.push_back((T)(0));
				item.push_back((T)(0));
			}

			target.insert(target.end(), item.begin(), item.end());

			vertex += size;
		}

		return target.size();
	}

	template <typename T>
	size_t copy(const Vertex& source, std::vector< std::vector<T> >& target, const VertexFormat format = xyz)
	{
		std::vector<float>::const_iterator vertex = source.v.begin();

		for (std::vector<int>::const_iterator it = source.s.begin(); it != source.s.end(); it++)
		{
			const int size = *it;

			std::vector<T> item;

			item.push_back((T)(size > 0 ? *(vertex + 0) : 0));
			item.push_back((T)(size > 1 ? *(vertex + 1) : 0));
			item.push_back((T)(size > 2 ? *(vertex + 2) : 0));

			if (format == xyzw)
			{
				item.push_back((T)(0));
			}

			if (format == xyzrgb)
			{
				item.push_back((T)(0));
				item.push_back((T)(0));
				item.push_back((T)(0));
			}

			target.push_back(item);

			vertex += size;
		}

		return target.size();
	}

	bool move(Normal& source, std::vector<float>& target)
	{
		if (source.empty()) return true;

		for (const auto& size : source.s)
		{
			if (size != xyz)
				return false;
		}

		target = std::move(source.v);

		source.clear();

		return true;
	}

	size_t copy(Normal& source, std::vector<float>& target)
	{
		if (move(source, target))
			return target.size();

		std::vector<float>::const_iterator normal = source.v.begin();

		for (std::vector<int>::const_iterator it = source.s.begin(); it != source.s.end(); it++)
		{
			const int size = *it;

			if (size == 3)
				target.insert(target.end(), normal, normal + size);
			else
			{
				std::vector<float> item;

				item.push_back(size > 0 ? *(normal + 0) : 0);
				item.push_back(size > 1 ? *(normal + 1) : 0);
				item.push_back(size > 2 ? *(normal + 2) : 0);

				target.insert(target.end(), item.begin(), item.end());
			}

			normal += size;
		}

		return target.size();
	}

	template <typename T>
	size_t copy(const Normal& source, std::vector<T>& target)
	{
		std::vector<float>::const_iterator normal = source.v.begin();

		for (std::vector<int>::const_iterator it = source.s.begin(); it != source.s.end(); it++)
		{
			const int size = *it;

			std::vector<T> item;

			item.push_back((T)(size > 0 ? *(normal + 0) : 0));
			item.push_back((T)(size > 1 ? *(normal + 1) : 0));
			item.push_back((T)(size > 2 ? *(normal + 2) : 0));

			target.insert(target.end(), item.begin(), item.end());

			normal += size;
		}

		return target.size();
	}

	template <typename T>
	size_t copy(const Normal& source, std::vector< std::vector<T> >& target)
	{
		std::vector<float>::const_iterator normal = source.v.begin();

		for (std::vector<int>::const_iterator it = source.s.begin(); it != source.s.end(); it++)
		{
			const int size = *it;

			std::vector<T> item;

			item.push_back((T)(size > 0 ? *(normal + 0) : 0));
			item.push_back((T)(size > 1 ? *(normal + 1) : 0));
			item.push_back((T)(size > 2 ? *(normal + 2) : 0));

			target.push_back(item);

			normal += size;
		}

		return target.size();
	}

	enum TextureFormat
	{
		uv = 2,
		uvw = 3
	};

	inline TextureFormat textureFormat(const size_t& size)
	{
		return size == 2 ? uv : uvw;
	}

	inline TextureFormat format(const Texture& texture, bool& varies)
	{
		varies = false;

		if (texture.empty()) return uv;

		const TextureFormat format = textureFormat(texture.s.front());

		for (std::vector<int>::const_iterator it = texture.s.begin(); it != texture.s.end(); it++)
		{
			const int size = *it;

			if (textureFormat(size) != format)
			{
				varies = true;

				return uv;
			}
		}

		return format;
	}

	bool move(Texture& source, std::vector<float>& target, const TextureFormat format = uvw)
	{
		bool varies(false);

		if (source.empty()) return true;

		if (obj::format(source, varies) == format && !varies)
		{
			target = std::move(source.v);

			source.clear();

			return true;
		}

		return false;
	}

	size_t copy(Texture& source, std::vector<float>& target, const TextureFormat format = uvw)
	{
		if (move(source, target, format))
			return target.size();

		std::vector<float>::const_iterator texture = source.v.begin();

		for (std::vector<int>::const_iterator it = source.s.begin(); it != source.s.end(); it++)
		{
			const int size = *it;

			if (format == size)
				target.insert(target.end(), texture, texture + size);
			else
			{
				std::vector<float> item;

				item.push_back(size > 0 ? *(texture + 0) : 0.0f);
				item.push_back(size > 1 ? *(texture + 1) : 0.0f);

				if (format == uvw)
					item.push_back(size > 2 ? *(texture + 2) : 1.0f);

				target.insert(target.end(), item.begin(), item.end());
			}

			texture += size;
		}

		return target.size();
	}

	template <typename T>
	size_t copy(const Texture& source, std::vector<T>& target, const TextureFormat format = uvw)
	{
		std::vector<float>::const_iterator texture = source.v.begin();

		for (std::vector<int>::const_iterator it = source.s.begin(); it != source.s.end(); it++)
		{
			const int size = *it;

			std::vector<T> item;

			item.push_back((T)(size > 0 ? *(texture + 0) : 0));
			item.push_back((T)(size > 1 ? *(texture + 1) : 0));

			if (format == uvw)
				item.push_back((T)(size > 2 ? *(texture + 2) : 1));

			target.insert(target.end(), item.begin(), item.end());

			texture += size;
		}

		return target.size();
	}

	template <typename T>
	size_t copy(const Texture& source, std::vector< std::vector<T> >& target, const TextureFormat format = uvw)
	{
		std::vector<float>::const_iterator texture = source.v.begin();

		for (std::vector<int>::const_iterator it = source.s.begin(); it != source.s.end(); it++)
		{
			const int size = *it;

			std::vector<T> item;

			item.push_back((T)(size > 0 ? *(texture + 0) : 0));
			item.push_back((T)(size > 1 ? *(texture + 1) : 0));

			if (format == uvw)
				item.push_back((T)(size > 2 ? *(texture + 2) : 1));

			target.push_back(item);

			texture += size;
		}

		return target.size();
	}
	
	// Face texture, normal or vertex indices
	inline size_t copy(const List<int>& source, std::vector< std::vector<int> >& target)
	{
		std::vector<int>::const_iterator item = source.v.begin();

		for (std::vector<int>::const_iterator it = source.s.begin(); it != source.s.end(); it++)
		{
			const int size = *it;

			target.push_back( std::vector<int>(item, item + size) );

			item += size;
		}

		return target.size();
	}
}

#endif // WAVEFRONT_OBJ
