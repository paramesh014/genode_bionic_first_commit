/*
 * \brief  Model-view synchronization
 * \author Genode Labs <qoost@genode-labs.com>
 * \date   2011-03-25
 *
 * Copyright (C) 2011 by secunet Security Networks AG
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#ifndef _INCLUDE__QOOST__MODEL_VIEW_H_
#define _INCLUDE__QOOST__MODEL_VIEW_H_

#include <qoost/update_container.h>
#include <qoost/meta.h>
#include <qoost/slot.h>

/**
 * The 'View_updater' updates a data _view_ according to the given data
 * _model_. View and model objects are expected to implement usual Qt4
 * container functions, i.e. are iteratable Qt4 containers with a 'value_type'.
 * For view modification functor types provided by the policy are used:
 *
 *   Item_creator
 *     Item_creator(View &);
 *     void operator() (Model::value_type model_item,
 *                      View::value_type view_item) const;
 *
 *   Item_disposer
 *     Item_disposer(View &);
 *     void operator() (View::value_type view_item) const;
 *
 *   Item_rearranger
 *     Item_rearranger(View &);
 *     void operator() (View::value_type view_item,
 *                      View::value_type view_item_succesor) const;
 *
 *   Item_updater
 *     Item_updater();
 *     void operator() (Model::value_type model_item,
 *                      View::value_type view_item) const;
 *
 * Additionally, 'View_updater' expects the following equality operator:
 *
 *   bool operator==(POLICY::Model::value_type const &,
 *                   POLICY::View::value_type const &);
 */

/**
 * Slot type provided by 'View_updater'
 */
class Update_view { };


/**
 * Aggregate of template arguments for 'View_updater'
 *
 * \param MODEL            model type
 * \param VIEW             view type
 * \param TYPE_MAP         association of model-item subtypes to view-item subtypes
 * \param ITEM_CREATOR     functor for creating a new view item
 * \param ITEM_DISPOSER    functor for disposal of old view items
 * \param ITEM_REARRANGER  functor for rearranging view items
 * \param ITEM_UPDATER     functor for updating a view item from its model item
 */
template <typename MODEL, typename VIEW, typename TYPE_MAP,
          typename ITEM_CREATOR, typename ITEM_DISPOSER,
          typename ITEM_REARRANGER, typename ITEM_UPDATER>
struct View_update_policy
{
	typedef MODEL           Model;
	typedef VIEW            View;
	typedef TYPE_MAP        Type_map;
	typedef ITEM_CREATOR    Item_creator;
	typedef ITEM_DISPOSER   Item_disposer;
	typedef ITEM_REARRANGER Item_rearranger;
	typedef ITEM_UPDATER    Item_updater;
};


/**
 * Model-to-view converter for populating and managing views
 */
template <typename POLICY>
class View_updater : public Slot<View_updater<POLICY>, Update_view>
{
	private:

		/**
		 * Shortcuts for internal use
		 */
		typedef View_updater<POLICY>                This;
		typedef typename POLICY::Model              Model;
		typedef typename POLICY::Model::value_type  Model_item;
		typedef typename POLICY::View               View;
		typedef typename POLICY::View::value_type   View_item;
		typedef typename POLICY::Type_map           Type_map;
		typedef typename POLICY::Item_creator       Item_creator;
		typedef typename POLICY::Item_disposer      Item_disposer;
		typedef typename POLICY::Item_rearranger    Item_rearranger;
		typedef typename POLICY::Item_updater       Item_updater;

		Model const &_model;
		View        &_view;

	public:

		void update() { update_container(*this, _model, _view); }

		/**
		 * Constructor
		 */
		View_updater(Model const &model, View &view)
		:
			Slot<This, Update_view>(&This::update),
			_model(model), _view(view)
		{ }


		/*****************************
		 ** Update-container policy **
		 *****************************/

		typedef Model In;
		typedef View  Out;

		bool match(Model_item const input_elem, View_item const output_elem)
		{
			return *input_elem == *output_elem;
		}

		void update(Model_item const input_elem, View_item output_elem)
		{
			for_each_type_association<Type_map>(input_elem, output_elem,
			                                    Item_updater());
		}

		void add_new(Model_item const input_elem)
		{
			for_each_type_association<Type_map>(input_elem, (View_item)0,
			                                    Item_creator(_view));
		}

		void remove_old(View_item output_elem)
		{
			(Item_disposer(_view))(output_elem);
		}

		void rearrange(View_item elem, View_item successor)
		{
			(Item_rearranger(_view))(elem, successor);
		}
};

#endif /* _INCLUDE__QOOST__MODEL_VIEW_H_ */
