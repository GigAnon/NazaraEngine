/*
	Nazara Engine

	Copyright (C) 2015 Jérôme "Lynix" Leclercq (Lynix680@gmail.com)

	Permission is hereby granted, free of charge, to any person obtaining a copy of
	this software and associated documentation files (the "Software"), to deal in
	the Software without restriction, including without limitation the rights to
	use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
	of the Software, and to permit persons to whom the Software is furnished to do
	so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/

#ifndef NAZARA_PREREQUESITES_HPP
#define NAZARA_PREREQUESITES_HPP

// Identification du compilateur
///TODO: Rajouter des tests d'identification de compilateurs
#if defined(__BORLANDC__)
	#define NAZARA_COMPILER_BORDLAND
	#define NAZARA_COMPILER_SUPPORTS_CPP11 (defined(__cplusplus) && __cplusplus >= 201103L)
	#define NAZARA_DEPRECATED(txt)
	#define NAZARA_FUNCTION __FUNC__
#elif defined(__clang__)
	#define NAZARA_COMPILER_CLANG
	#define NAZARA_COMPILER_SUPPORTS_CPP11 (defined(__cplusplus) && __cplusplus >= 201103L)
	#define NAZARA_DEPRECATED(txt) __attribute__((__deprecated__(txt)))
	#define NAZARA_FUNCTION __PRETTY_FUNCTION__
#elif defined(__GNUC__) || defined(__MINGW32__)
	#define NAZARA_COMPILER_GCC
	#define NAZARA_COMPILER_SUPPORTS_CPP11 (defined(__cplusplus) && __cplusplus >= 201103L)
	#define NAZARA_DEPRECATED(txt) __attribute__((__deprecated__(txt)))
	#define NAZARA_FUNCTION __PRETTY_FUNCTION__

	#ifdef __MINGW32__
		#define NAZARA_COMPILER_MINGW
		#ifdef __MINGW64_VERSION_MAJOR
			#define NAZARA_COMPILER_MINGW_W64
		#endif
	#endif
#elif defined(__INTEL_COMPILER) || defined(__ICL)
	#define NAZARA_COMPILER_INTEL
	#define NAZARA_COMPILER_SUPPORTS_CPP11 (defined(__cplusplus) && __cplusplus >= 201103L)
	#define NAZARA_DEPRECATED(txt)
	#define NAZARA_FUNCTION __FUNCTION__
#elif defined(_MSC_VER)
	#define NAZARA_COMPILER_MSVC
	#define NAZARA_COMPILER_SUPPORTS_CPP11 (_MSC_VER >= 1900)
	#define NAZARA_DEPRECATED(txt) __declspec(deprecated(txt))
	#define NAZARA_FUNCTION __FUNCSIG__

	#pragma warning(disable: 4251)
#else
	#define NAZARA_COMPILER_UNKNOWN
	#define NAZARA_DEPRECATED(txt)
	#define NAZARA_FUNCTION __func__ // __func__ est standard depuis le C++11

	/// Cette ligne n'est là que pour prévenir, n'hésitez pas à la commenter si elle vous empêche de compiler
	#pragma message This compiler is not fully supported
#endif

#if !NAZARA_COMPILER_SUPPORTS_CPP11
	#error Nazara requires a C++11 compliant compiler
#endif

// Version du moteur
#define NAZARA_VERSION_MAJOR 0
#define NAZARA_VERSION_MINOR 1

#include <Nazara/Core/Config.hpp>

// Identification de la plateforme
#if defined(_WIN32)
	#define NAZARA_PLATFORM_WINDOWS

	#define NAZARA_EXPORT __declspec(dllexport)
	#define NAZARA_IMPORT __declspec(dllimport)

	// Des defines pour le header Windows
	#if defined(NAZARA_BUILD) // Pour ne pas entrer en conflit avec les defines de l'application ou d'une autre bibliothèque
		#ifndef WIN32_LEAN_AND_MEAN
			#define WIN32_LEAN_AND_MEAN
		#endif

		#ifndef NOMINMAX
			#define NOMINMAX
		#endif

		#if NAZARA_CORE_WINDOWS_VISTA
			// Version de Windows minimale : Vista
			#define NAZARA_WINNT 0x0600
		#else
			#define NAZARA_WINNT 0x0501
		#endif

		// Pour ne pas casser le define déjà en place s'il est applicable
		#if defined(_WIN32_WINNT)
			#if _WIN32_WINNT < NAZARA_WINNT
				#undef _WIN32_WINNT
				#define _WIN32_WINNT NAZARA_WINNT
			#endif
		#else
			#define _WIN32_WINNT NAZARA_WINNT
		#endif
	#endif
#elif defined(__linux__) || defined(__unix__)
	#define NAZARA_PLATFORM_LINUX
	#define NAZARA_PLATFORM_GLX
	#define NAZARA_PLATFORM_POSIX
	#define NAZARA_PLATFORM_X11

	#define NAZARA_EXPORT __attribute__((visibility ("default")))
	#define NAZARA_IMPORT __attribute__((visibility ("default")))
/*#elif defined(__APPLE__) && defined(__MACH__)
	#define NAZARA_CORE_API
	#define NAZARA_PLATFORM_MACOSX
	#define NAZARA_PLATFORM_POSIX*/
#else
	// À commenter pour tenter quand même une compilation
	#error This operating system is not fully supported by the Nazara Engine

	#define NAZARA_PLATFORM_UNKNOWN
	#define NAZARA_CORE_API
#endif

// Détection 64 bits
#if !defined(NAZARA_PLATFORM_x64) && (defined(_WIN64) ||  defined(__amd64__) || defined(__x86_64__) || defined(__ia64__) || defined(__ia64) || \
    defined(_M_IA64) || defined(__itanium__) || defined(__MINGW64__) || defined(_M_AMD64) || defined (_M_X64))
	#define NAZARA_PLATFORM_x64
#endif

// Définit NDEBUG si NAZARA_DEBUG n'est pas présent
#if !defined(NAZARA_DEBUG) && !defined(NDEBUG)
	#define NDEBUG
#endif

// Macros supplémentaires
#define NazaraPrefix(a, prefix) prefix ## a
#define NazaraPrefixMacro(a, prefix) NazaraPrefix(a, prefix)
#define NazaraSuffix(a, suffix) a ## suffix
#define NazaraSuffixMacro(a, suffix) NazaraSuffix(a, suffix)
#define NazaraStringify(s) #s
#define NazaraStringifyMacro(s) NazaraStringify(s) // http://gcc.gnu.org/onlinedocs/cpp/Stringification.html#Stringification
#define NazaraUnused(a) (void) a

#include <cstdint>

static_assert(sizeof(int8_t)  == 1, "int8_t is not of the correct size" );
static_assert(sizeof(int16_t) == 2, "int16_t is not of the correct size");
static_assert(sizeof(int32_t) == 4, "int32_t is not of the correct size");
static_assert(sizeof(int64_t) == 8, "int64_t is not of the correct size");

static_assert(sizeof(uint8_t)  == 1, "uint8_t is not of the correct size" );
static_assert(sizeof(uint16_t) == 2, "uint16_t is not of the correct size");
static_assert(sizeof(uint32_t) == 4, "uint32_t is not of the correct size");
static_assert(sizeof(uint64_t) == 8, "uint64_t is not of the correct size");

namespace Nz
{
    typedef int8_t Int8;
    typedef uint8_t UInt8;

    typedef int16_t Int16;
    typedef uint16_t UInt16;

    typedef int32_t Int32;
    typedef uint32_t UInt32;

    typedef int64_t Int64;
    typedef uint64_t UInt64;
}

#endif // NAZARA_PREREQUESITES_HPP
