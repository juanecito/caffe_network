/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * CCifar10.hpp
 * Copyright (C) 2017 Juan Maria Gomez Lopez <juanecitorr@gmail.com>
 *
 * caffe_network is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * caffe_wgan is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/** @file CCifar10.hpp
 * @author Juan Maria Gomez Lopez <juanecitorr@gmail.com>
 * @date 02 Jun 2017
 */

#ifndef SRC_CCIFAR10_HPP_
#define SRC_CCIFAR10_HPP_

#include <vector>
#include <memory>
#include <map>
#include <iostream>
#include <string>
#include <cstring>

#include <cuda.h>
#include <functional>

/**
 *
 */
template <typename DataType>
struct S_Cifar10_img_rgb
{
	DataType rgb_[3072];
}__attribute__((packed));

/**
 *
 */
struct S_Cifar10_label_img
{
	uint8_t label_;
	uint8_t red_channel_[1024];
	uint8_t green_channel_[1024];
	uint8_t blue_channel_[1024];
}__attribute__((packed));

template <typename DataType>
struct S_Cifar10_img
{
	DataType red_channel_[1024];
	DataType green_channel_[1024];
	DataType blue_channel_[1024];
}__attribute__((packed));

////////////////////////////////////////////////////////////////////////////////
/**
 *
 */
class CCifar10
{

	public:

		CCifar10();
		virtual ~CCifar10();


		void set_path(const std::string& path) {path_ = path;}

		void load_train_batch_by_index(unsigned int train_batch_index,
										struct S_Cifar10_label_img* lb_imgs,
										S_Cifar10_img_rgb<uint8_t>* img,
										uint8_t* labels);

		void load_train_batchs(void);

		void load_test_batch_by_index(unsigned int train_batch_index,
										struct S_Cifar10_label_img* lb_imgs,
										S_Cifar10_img_rgb<uint8_t>* img,
										uint8_t* labels);

		void load_test_batchs(void);

		//----------------------------------------------------------------------
		template <typename T>
		size_t get_batch_imgs_size(void) const
		{
			return sizeof(struct S_Cifar10_img<T>) * cifar10_imgs_batch_s;
		}

		template <typename T>
		size_t get_batch_labels_size(void) const
		{
			return sizeof(T) * cifar10_imgs_batch_s;
		}
		//----------------------------------------------------------------------
		// Get RGB images
		/**
		 *
		 * @param batch_index
		 * @param img_index
		 * @return
		 */
		uint8_t* get_train_img_rgb(unsigned int batch_index, unsigned int img_index)
		{
			struct S_Cifar10_img_rgb<uint8_t>* imgs = train_batchs_.at(batch_index).get();
			return imgs[img_index].rgb_;
		}

		/**
		 *
		 * @param img_index
		 * @return
		 */
		uint8_t* get_test_img_rgb(unsigned int img_index)
		{
			struct S_Cifar10_img_rgb<uint8_t>* imgs = test_batchs_.at(0).get();
			return imgs[img_index].rgb_;
		}
		//----------------------------------------------------------------------
		// Get images without labels
		/**
		 *
		 * @param batch_index
		 * @return
		 */
		template <typename T>
		unsigned int __attribute__((warn_unused_result))
			get_train_batch_img(unsigned int batch_index, T** imgs);

		template <typename T>
		unsigned int __attribute__((warn_unused_result))
			get_train_batch_img(unsigned int batch_index, T** imgs,
						const std::vector<std::function<unsigned int(T**, unsigned int)>>& vector_transf);

		template <typename T>
		unsigned int __attribute__((warn_unused_result))
			get_train_batch_img_by_label(uint8_t label_index, T** imgs);

		template <typename T>
		unsigned int __attribute__((warn_unused_result))
			get_train_batch_img_by_label(uint8_t label_index, T** imgs,
						const std::vector<std::function<unsigned int(T**, unsigned int)>>& vector_transf);

		template <typename T>
		unsigned int get_all_train_batch_img(T** imgs);

		template <typename T>
		unsigned int __attribute__((warn_unused_result))
			get_all_train_batch_img(T** imgs,
						const std::vector<std::function<unsigned int(T**, unsigned int)>>& vector_transf);

		template <typename T>
		unsigned int __attribute__((warn_unused_result))
			get_all_train_batch_img_rgb(T** imgs);

		/**
		 *
		 * @return
		 */
		template <typename T>
		unsigned int __attribute__((warn_unused_result)) get_test_batch_img(T** imgs);

		template <typename T>
		unsigned int __attribute__((warn_unused_result)) get_all_test_batch_img(T** imgs);

		template <typename T>
		unsigned int __attribute__((warn_unused_result)) get_all_test_batch_img(T** imgs,
						const std::vector<std::function<unsigned int(T**, unsigned int)>>& vector_transf);

		template <typename T>
		unsigned int __attribute__((warn_unused_result)) get_all_test_batch_img_rgb(T** imgs);
		//----------------------------------------------------------------------
		// Get images with label, as we can find in cifar10 files
		/**
		 *
		 * @param batch_index
		 * @param img_index
		 * @return
		 */
		uint8_t* get_ori_train_img(unsigned int batch_index, unsigned int img_index)
		{
			struct S_Cifar10_label_img* imgs = ori_train_batchs_.at(batch_index).get();
			return (uint8_t*)imgs;
		}

		//----------------------------------------------------------------------
		// Get labels
		void get_train_labels(unsigned int batch_index, uint8_t** t_labels)
		{
			*t_labels = new uint8_t [cifar10_imgs_batch_s];
			memcpy(*t_labels, this->train_labels_.at(batch_index).get(),
							sizeof(uint8_t) * cifar10_imgs_batch_s);
		}

		void get_test_labels(uint8_t** t_labels)
		{
			*t_labels = new uint8_t [cifar10_imgs_batch_s];
			memcpy(*t_labels, this->test_labels_.at(0).get(),
							sizeof(uint8_t) * cifar10_imgs_batch_s);
		}

		template <typename T>
		void get_train_labels(unsigned int batch_index, T** t_labels);

		template <typename T>
		unsigned int get_all_train_labels(T** t_labels);

		template <typename T>
		void get_test_labels(T** t_labels);

		template <typename T>
		unsigned int get_all_test_labels(T** t_labels);
		//----------------------------------------------------------------------
		/**
		 *
		 * @param img_index
		 * @return
		 */
		uint8_t* get_ori_test_img(unsigned int img_index)
		{
			struct S_Cifar10_label_img* imgs = ori_test_batchs_.at(0).get();
			return (uint8_t*)imgs;
		}

		void show_train_img(unsigned int batch_index, unsigned int img_index);
		void show_test_img(unsigned int img_index);
		void show_img(uint8_t* img, size_t img_size);
		static void print_cifar10_labels(void)
		{
			for (auto it : CCifar10::cifar10_labels)
			{
				std::cout << "(" << (int)(it.first) << ":" << it.second << ") ";
			}
			std::cout << std::endl;
		}

		const static unsigned int cifar10_imgs_batch_s = 10000;
		const static unsigned int cifar10_train_batch_s = 5;
		const static unsigned int cifar10_test_batch_s = 1;

		static const std::string train_batch_pattern_name_s;
		static const std::string test_batch_pattern_name_s;
		const static std::map<uint8_t, std::string> cifar10_labels;

	private:

		void calculate_means(void);

		/*!
		 *
		 */
		bool is_train_loaded_;

		bool is_test_loaded_;

		std::string path_;

		std::vector<std::shared_ptr<uint8_t> > train_labels_;
		std::vector<std::shared_ptr<uint8_t> > test_labels_;

		std::vector<std::shared_ptr<struct S_Cifar10_label_img> > ori_train_batchs_;
		std::vector<std::shared_ptr<struct S_Cifar10_label_img> > ori_test_batchs_;

		std::vector<std::shared_ptr<struct S_Cifar10_img_rgb<uint8_t> > > train_batchs_;
		std::vector<std::shared_ptr<struct S_Cifar10_img_rgb<uint8_t> > > test_batchs_;

		struct S_Cifar10_img<double> mean_values_;
};



////////////////////////////////////////////////////////////////////////////////
template <typename T>
unsigned int CCifar10::get_train_batch_img(unsigned int batch_index, T** imgs)
{
	struct S_Cifar10_img<T>* tmp_img = new struct S_Cifar10_img<T> [cifar10_imgs_batch_s];
	struct S_Cifar10_label_img* ori_imgs = ori_train_batchs_.at(batch_index).get();

	for (unsigned int uiI = 0; uiI < cifar10_imgs_batch_s; uiI++)
	{
		for (unsigned int uiJ = 0; uiJ < 1024; uiJ++)
		{
			tmp_img[uiI].red_channel_[uiJ] = ori_imgs[uiI].red_channel_[uiJ];
			tmp_img[uiI].green_channel_[uiJ] = ori_imgs[uiI].green_channel_[uiJ];
			tmp_img[uiI].blue_channel_[uiJ] = ori_imgs[uiI].blue_channel_[uiJ];
		}
	}
	*imgs = (T*)(ori_imgs);

	return cifar10_imgs_batch_s;
}

////////////////////////////////////////////////////////////////////////////////
template <typename T>
unsigned int CCifar10::get_train_batch_img(unsigned int batch_index, T** imgs,
				const std::vector<std::function<unsigned int(T**, unsigned int)>>& vector_transf)
{
	unsigned int count = this->get_train_batch_img(batch_index, imgs);

	for (auto it_fn : vector_transf)
	{
		count = it_fn(imgs, count);
	}

	return count;
}

////////////////////////////////////////////////////////////////////////////////
template <typename T>
unsigned int __attribute__((warn_unused_result))
	CCifar10::get_train_batch_img_by_label(uint8_t label_index, T** imgs)
{
	struct S_Cifar10_img<T> tmp_img;

	std::vector<struct S_Cifar10_img<T> > img_in_label;
	img_in_label.clear();
	unsigned int count = 0;

	for (auto it_batch : ori_train_batchs_)
	{
		struct S_Cifar10_label_img* ori_imgs = it_batch.get();

		for (unsigned int uiI = 0; uiI < cifar10_imgs_batch_s; uiI++)
		{
			if (ori_imgs[uiI].label_ == label_index)
			{
				for (unsigned int uiJ = 0; uiJ < 1024; uiJ++)
				{
					tmp_img.red_channel_[uiJ] = ori_imgs[uiI].red_channel_[uiJ];
					tmp_img.green_channel_[uiJ] = ori_imgs[uiI].green_channel_[uiJ];
					tmp_img.blue_channel_[uiJ] = ori_imgs[uiI].blue_channel_[uiJ];
				}

				img_in_label.push_back(tmp_img);
				count++;
			}
		}
	}

	*imgs = new T [img_in_label.size() * sizeof(struct S_Cifar10_img<T>)];
	memcpy(*imgs, img_in_label.data(), img_in_label.size() * sizeof(struct S_Cifar10_img<T>));

	return count;
}

////////////////////////////////////////////////////////////////////////////////
template <typename T>
unsigned int __attribute__((warn_unused_result))
	CCifar10::get_train_batch_img_by_label(uint8_t label_index, T** imgs,
				const std::vector<std::function<unsigned int(T**, unsigned int)>>& vector_transf)
{
	unsigned int count = this->get_train_batch_img_by_label<T>(label_index, imgs);

	for (auto it_fn : vector_transf)
	{
		count = it_fn(imgs, count);
	}

	return count;
}

////////////////////////////////////////////////////////////////////////////////
template <typename T>
unsigned int CCifar10::get_all_train_batch_img(T** imgs,
				const std::vector<std::function<unsigned int(T**, unsigned int)>>& vector_transf)
{
	unsigned int count = this->get_all_train_batch_img<T>(imgs);

	for (auto it_fn : vector_transf)
	{
		count = it_fn(imgs, count);
	}

	return count;
}

////////////////////////////////////////////////////////////////////////////////
template <typename T>
unsigned int CCifar10::get_all_train_batch_img(T** imgs)
{
	unsigned int count = cifar10_imgs_batch_s * ori_train_batchs_.size();

	struct S_Cifar10_img<T>* tmp_img = new struct S_Cifar10_img<T> [count];

	unsigned int batch_index = 0;
	for (auto it : ori_train_batchs_)
	{
		struct S_Cifar10_label_img* ori_imgs = it.get();

		for (unsigned int uiI = 0; uiI < cifar10_imgs_batch_s; uiI++)
		{
			for (unsigned int uiJ = 0; uiJ < 1024; uiJ++)
			{
				tmp_img[(batch_index * cifar10_imgs_batch_s) + uiI].red_channel_[uiJ] = ori_imgs[uiI].red_channel_[uiJ];
				tmp_img[(batch_index * cifar10_imgs_batch_s) + uiI].green_channel_[uiJ] = ori_imgs[uiI].green_channel_[uiJ];
				tmp_img[(batch_index * cifar10_imgs_batch_s) + uiI].blue_channel_[uiJ] = ori_imgs[uiI].blue_channel_[uiJ];
			}
		}
		batch_index++;
	}

	*imgs = (T*)(tmp_img);

	return count;
}

////////////////////////////////////////////////////////////////////////////////
template <typename T>
unsigned int CCifar10::get_all_train_batch_img_rgb(T** imgs)
{
	unsigned int count = cifar10_imgs_batch_s * ori_train_batchs_.size();
	struct S_Cifar10_img_rgb<T>* tmp_img = new struct S_Cifar10_img_rgb<T> [count];

	unsigned int batch_index = 0;
	for (auto it : ori_train_batchs_)
	{
		struct S_Cifar10_label_img* ori_imgs = it.get();

		for (unsigned int uiI = 0; uiI < cifar10_imgs_batch_s; uiI++)
		{
			for (unsigned int uiJ = 0; uiJ < 1024; uiJ++)
			{
				tmp_img[(batch_index * cifar10_imgs_batch_s) + uiI].rgb_[uiJ * 3] = ori_imgs[uiI].red_channel_[uiJ];
				tmp_img[(batch_index * cifar10_imgs_batch_s) + uiI].rgb_[uiJ * 3 + 1] = ori_imgs[uiI].green_channel_[uiJ];
				tmp_img[(batch_index * cifar10_imgs_batch_s) + uiI].rgb_[uiJ * 3 + 2] = ori_imgs[uiI].blue_channel_[uiJ];
			}
		}
		batch_index++;
	}

	*imgs = (T*)(tmp_img);

	return count;
}

////////////////////////////////////////////////////////////////////////////////
template <typename T>
unsigned int CCifar10::get_test_batch_img(T** imgs)
{
	struct S_Cifar10_img<T>* tmp_img = new struct S_Cifar10_img<T> [cifar10_imgs_batch_s];
	struct S_Cifar10_label_img* ori_imgs = ori_test_batchs_.at(0).get();

	for (unsigned int uiI = 0; uiI < cifar10_imgs_batch_s; uiI++)
	{
		for (unsigned int uiJ = 0; uiJ < 1024; uiJ++)
		{
			tmp_img[uiI].red_channel_[uiJ] = ori_imgs[uiI].red_channel_[uiJ];
			tmp_img[uiI].green_channel_[uiJ] = ori_imgs[uiI].green_channel_[uiJ];
			tmp_img[uiI].blue_channel_[uiJ] = ori_imgs[uiI].blue_channel_[uiJ];
		}
	}
	*imgs = (T*)(ori_imgs);

	return cifar10_imgs_batch_s;
}

////////////////////////////////////////////////////////////////////////////////
template <typename T>
unsigned int CCifar10::get_all_test_batch_img(T** imgs)
{
	unsigned int count = cifar10_imgs_batch_s * ori_test_batchs_.size();

	struct S_Cifar10_img<T>* tmp_img = new struct S_Cifar10_img<T> [count];

	unsigned int batch_index = 0;
	for (auto it : ori_test_batchs_)
	{
		struct S_Cifar10_label_img* ori_imgs = it.get();

		for (unsigned int uiI = 0; uiI < cifar10_imgs_batch_s; uiI++)
		{
			for (unsigned int uiJ = 0; uiJ < 1024; uiJ++)
			{
				tmp_img[(batch_index * cifar10_imgs_batch_s) + uiI].red_channel_[uiJ] = ori_imgs[uiI].red_channel_[uiJ];
				tmp_img[(batch_index * cifar10_imgs_batch_s) + uiI].green_channel_[uiJ] = ori_imgs[uiI].green_channel_[uiJ];
				tmp_img[(batch_index * cifar10_imgs_batch_s) + uiI].blue_channel_[uiJ] = ori_imgs[uiI].blue_channel_[uiJ];
			}
		}
		batch_index++;
	}

	*imgs = (T*)(tmp_img);

	return count;
}

////////////////////////////////////////////////////////////////////////////////
template <typename T>
unsigned int CCifar10::get_all_test_batch_img(T** imgs,
				const std::vector<std::function<unsigned int(T**, unsigned int)>>& vector_transf)
{
	unsigned int count = this->get_all_test_batch_img<T>(imgs);

	for (auto it_fn : vector_transf)
	{
		count = it_fn(imgs, count);
	}

	return count;
}

////////////////////////////////////////////////////////////////////////////////
template <typename T>
unsigned int CCifar10::get_all_test_batch_img_rgb(T** imgs)
{
	unsigned int count = cifar10_imgs_batch_s * ori_test_batchs_.size();
	struct S_Cifar10_img_rgb<T>* tmp_img = new struct S_Cifar10_img_rgb<T> [count];

	unsigned int batch_index = 0;
	for (auto it : ori_test_batchs_)
	{
		struct S_Cifar10_label_img* ori_imgs = it.get();

		for (unsigned int uiI = 0; uiI < cifar10_imgs_batch_s; uiI++)
		{
			for (unsigned int uiJ = 0; uiJ < 1024; uiJ++)
			{
				tmp_img[(batch_index * cifar10_imgs_batch_s) + uiI].rgb_[uiJ * 3] = ori_imgs[uiI].red_channel_[uiJ];
				tmp_img[(batch_index * cifar10_imgs_batch_s) + uiI].rgb_[uiJ * 3 + 1] = ori_imgs[uiI].green_channel_[uiJ];
				tmp_img[(batch_index * cifar10_imgs_batch_s) + uiI].rgb_[uiJ * 3 + 2] = ori_imgs[uiI].blue_channel_[uiJ];
			}
		}
		batch_index++;
	}

	*imgs = (T*)(tmp_img);

	return count;
}

////////////////////////////////////////////////////////////////////////////////
template <typename T>
void CCifar10::get_train_labels(unsigned int batch_index, T** t_labels)
{
	*t_labels = new T [cifar10_imgs_batch_s];
	uint8_t* train_label = this->train_labels_.at(batch_index).get();
	for (unsigned int uiI = 0; uiI < cifar10_imgs_batch_s; uiI++)
	{
		(*t_labels)[uiI] = train_label[uiI];
	}
}

////////////////////////////////////////////////////////////////////////////////
template <typename T>
void CCifar10::get_test_labels(T** t_labels)
{
	*t_labels = new T [cifar10_imgs_batch_s];
	uint8_t* test_label = this->train_labels_.at(0).get();
	for (unsigned int uiI = 0; uiI < cifar10_imgs_batch_s; uiI++)
	{
		(*t_labels)[uiI] = test_label[uiI];
	}
}

////////////////////////////////////////////////////////////////////////////////
template <typename T>
unsigned int CCifar10::get_all_train_labels(T** t_labels)
{
	unsigned int count = cifar10_imgs_batch_s * train_labels_.size();
	*t_labels = new T [count];

	unsigned int batch_index = 0;
	for (auto it : train_labels_)
	{
		for (unsigned int uiI = 0; uiI < cifar10_imgs_batch_s; uiI++)
		{
			(*t_labels)[batch_index * cifar10_imgs_batch_s + uiI] = it.get()[uiI];
		}
		batch_index++;
	}

	return count;
}

////////////////////////////////////////////////////////////////////////////////
template <typename T>
unsigned int CCifar10::get_all_test_labels(T** t_labels)
{
	unsigned int count = cifar10_imgs_batch_s * test_labels_.size();
	*t_labels = new T [count];

	unsigned int batch_index = 0;
	for (auto it : test_labels_)
	{
		for (unsigned int uiI = 0; uiI < cifar10_imgs_batch_s; uiI++)
		{
			(*t_labels)[batch_index * cifar10_imgs_batch_s + uiI] = it.get()[uiI];
		}
		batch_index++;
	}

	return count;
}


#endif /* SRC_CCIFAR10_HPP_ */
