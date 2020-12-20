//
// Created by Konstantin Rybkin on 08/11/2019.
//

#ifndef SRC_ITERABLE_CONVERTER_H
#define SRC_ITERABLE_CONVERTER_H

struct iterable_converter {
    template<typename Container>
    iterable_converter &
    from_python() {
        boost::python::converter::registry::push_back(
                &iterable_converter::convertible,
                &iterable_converter::construct<Container>,
                boost::python::type_id<Container>());

        return *this;
    }

    static void *convertible(PyObject *object) {
        return PyObject_GetIter(object) ? object : NULL;
    }

    template<typename Container>
    static void construct(
            PyObject *object,
            boost::python::converter::rvalue_from_python_stage1_data *data) {
        namespace python = boost::python;
        python::handle<> handle(python::borrowed(object));

        typedef python::converter::rvalue_from_python_storage<Container> storage_type;
        void *storage = reinterpret_cast<storage_type *>(data)->storage.bytes;

        typedef python::stl_input_iterator<typename Container::value_type>
                iterator;

        new(storage) Container(
                iterator(python::object(handle)),
                iterator());
        data->convertible = storage;
    }

    template<typename Container>
    struct vec_to_list_converter {
        static PyObject* convert(Container const& vec) {
            boost::python::list pyList;
            for (auto item: vec) {
                pyList.append(item);
            }
            return boost::python::incref(pyList.ptr());
        }
    };

    template<typename Container>
    iterable_converter &to_python() {
        boost::python::to_python_converter<Container, vec_to_list_converter<Container>>();
        return *this;
    }
};

#endif //SRC_ITERABLE_CONVERTER_H
