//Copyright (c) 2013-2014, The MercuryDPM Developers Team. All rights reserved.
//For the list of developers, see <http://www.MercuryDPM.org/Team>.
//
//Redistribution and use in source and binary forms, with or without
//modification, are permitted provided that the following conditions are met:
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//  * Neither the name MercuryDPM nor the
//    names of its contributors may be used to endorse or promote products
//    derived from this software without specific prior written permission.
//
//THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
//ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
//WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
//DISCLAIMED. IN NO EVENT SHALL THE MERCURYDPM DEVELOPERS TEAM BE LIABLE FOR ANY
//DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
//(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
//LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
//ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
//SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "HDF5.h"

#include <iostream> 
#include <string>
const int MH5_ATTR_VERSION = 1;


HDF5File::HDF5File(std::string filename, bool create)
{
    if (create && H5Fis_hdf5(filename.c_str()) < 0) {
        fileDescriptor_ = H5Fcreate(filename.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
//        if (fileDescriptor_ > 0)
//            setupInitialStructure();
    } else {
        fileDescriptor_ = H5Fopen(filename.c_str(), H5F_ACC_RDWR, H5P_DEFAULT);
        
    }
}

HDF5File::~HDF5File()
{
    if (fileDescriptor_ > 0) {
        std::cout << "LEAK SUMMARY: (File: should be 1)"
                  << "\nLOCAL: "
                  << "\n\tFiles:      " << H5Fget_obj_count(fileDescriptor_, H5F_OBJ_FILE | H5F_OBJ_LOCAL)
                  << "\n\tDatasets:   " << H5Fget_obj_count(fileDescriptor_, H5F_OBJ_DATASET | H5F_OBJ_LOCAL)
                  << "\n\tGroups:     " << H5Fget_obj_count(fileDescriptor_, H5F_OBJ_GROUP | H5F_OBJ_LOCAL)
                  << "\n\tDatatype:   " << H5Fget_obj_count(fileDescriptor_, H5F_OBJ_DATATYPE | H5F_OBJ_LOCAL)
                  << "\n\tAttributes: " << H5Fget_obj_count(fileDescriptor_, H5F_OBJ_ATTR | H5F_OBJ_LOCAL) << std::endl;
        std::cout << "GLOBAL: "
                  << "\n\tFiles:      " << H5Fget_obj_count(fileDescriptor_, H5F_OBJ_FILE)
                  << "\n\tDatasets:   " << H5Fget_obj_count(fileDescriptor_, H5F_OBJ_DATASET)
                  << "\n\tGroups:     " << H5Fget_obj_count(fileDescriptor_, H5F_OBJ_GROUP)
                  << "\n\tDatatype:   " << H5Fget_obj_count(fileDescriptor_, H5F_OBJ_DATATYPE)
                  << "\n\tAttributes: " << H5Fget_obj_count(fileDescriptor_, H5F_OBJ_ATTR) << std::endl;
        std::cout << "Grand total:"
                  << "\n\tALL:        " << H5Fget_obj_count(fileDescriptor_, H5F_OBJ_ALL) << std::endl;
        H5Fclose(fileDescriptor_);
    }
}

HDF5File::operator bool() const {
    return fileDescriptor_ > 0;
}

hid_t HDF5File::getHID() const {
    return fileDescriptor_;
}