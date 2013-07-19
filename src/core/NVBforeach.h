//
// Copyright 2011 - 2013 Timofey <typograph@elec.ru>
//
// This file is part of Novembre utility library.
//
// Novembre utility library is free software: you can redistribute it
// and/or modify it  under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation, either version 2
// of the License, or (at your option) any later version.
//
// Novembre is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#ifndef NVB_FOREACH
//
// Code taken from Qt 4.6.2 qglobal.h
// and changed to allow const pointer containers
//
#if defined(Q_CC_GNU) && !defined(Q_CC_INTEL) && !defined(Q_CC_RVCT)
/* make use of typeof-extension */
template <typename T>
class NVBForeachContainer {
	public:
		inline NVBForeachContainer(const T * t) : c(t), brk(0), i(c->begin()), e(c->end()) { }
		const T * c;
		int brk;
		typename T::const_iterator i, e;
	};

#define NVB_FOREACH(variable, container)                                \
for (NVBForeachContainer<__typeof__(*container)> _container_(container); \
     !_container_.brk && _container_.i != _container_.e;              \
     __extension__  ({ ++_container_.brk; ++_container_.i; }))                       \
    for (variable = *_container_.i;; __extension__ ({--_container_.brk; break;}))

#else

struct NVBForeachContainerBase {};

template <typename T>
class NVBForeachContainer : public NVBForeachContainerBase {
	public:
		inline NVBForeachContainer(const T * t): c(t), brk(0), i(c->begin()), e(c->end()) {};
		const T * c;
		mutable int brk;
		mutable typename T::const_iterator i, e;
		inline bool condition() const { return (!brk++ && i != e); }
	};

template <typename T> inline T * NVBForeachPointer(const T *) { return 0; }

template <typename T> inline NVBForeachContainer<T> NVBForeachContainerNew(const T * t)
	{ return NVBForeachContainer<T>(t); }

template <typename T>
inline const NVBForeachContainer<T> *NVBGetForeachContainer(const NVBForeachContainerBase *base, const T *)
	{ return static_cast<const NVBForeachContainer<T> *>(base); }

#if (defined(Q_CC_MSVC) && !defined(Q_CC_MSVC_NET) && !defined(Q_CC_INTEL)) || defined(Q_CC_MIPS)
/*
   Proper for-scoping in VC++6 and MIPSpro CC
*/
#  define NVB_FOREACH(variable,container)                                                             \
    if(0){}else                                                                                     \
    for (const NVBForeachContainerBase &_container_ = NVBForeachContainerNew(container);                \
         NVBForeachContainer(&_container_, true ? 0 : NVBForeachPointer(container))->condition();       \
         ++NVBForeachContainer(&_container_, true ? 0 : NVBForeachPointer(container))->i)               \
        for (variable = *NVBForeachContainer(&_container_, true ? 0 : NVBForeachPointer(container))->i; \
             NVBForeachContainer(&_container_, true ? 0 : NVBForeachPointer(container))->brk;           \
             --NVBForeachContainer(&_container_, true ? 0 : NVBForeachPointer(container))->brk)

#elif defined(Q_CC_DIAB)
// VxWorks DIAB generates unresolvable symbols, if container is a function call
#  define NVB_FOREACH(variable,container)                                                             \
    if(0){}else                                                                                     \
    for (const NVBForeachContainerBase &_container_ = NVBForeachContainerNew(container);                \
         NVBGetForeachContainer(&_container_, (__typeof__(*container)) 0)->condition();       \
         ++NVBGetForeachContainer(&_container_, (__typeof__(*container)) 0)->i)               \
        for (variable = *NVBGetForeachContainer(&_container_, (__typeof__(*container)) 0)->i; \
             NVBGetForeachContainer(&_container_, (__typeof__(*container)) 0)->brk;           \
             --NVBGetForeachContainer(&_container_, (__typeof__(*container)) 0)->brk)

#else
#  define NVB_FOREACH(variable, container) \
    for (const NVBForeachContainerBase &_container_ = NVBForeachContainerNew(container); \
         NVBGetForeachContainer(&_container_, true ? 0 : NVBForeachPointer(container))->condition();       \
         ++NVBGetForeachContainer(&_container_, true ? 0 : NVBForeachPointer(container))->i)               \
        for (variable = *NVBForeachContainer(&_container_, true ? 0 : NVBForeachPointer(container))->i; \
             NVBGetForeachContainer(&_container_, true ? 0 : NVBForeachPointer(container))->brk;           \
             --NVBGetForeachContainer(&_container_, true ? 0 : NVBForeachPointer(container))->brk)
#endif // MSVC6 || MIPSpro

#endif

#endif