#ifndef wave_target_hpp
#define wave_target_hpp

#include <algorithm>
#include <vector>

#include "Wave.hpp"

#include <Coupler_Point.hpp>
#include <Coupler_DataTarget.hpp>

#include "Teuchos_ArrayView.hpp"
#include "Teuchos_RCP.hpp"
#include "Teuchos_Comm.hpp"

//---------------------------------------------------------------------------//
namespace Coupler {

// DataTarget interface implementation for the Wave code.
template<class DataType_T, class HandleType_T, class CoordinateType_T>
class Wave_DataTarget 
    : public DataTarget<DataType_T, HandleType_T, CoordinateType_T>
{
  public:

    typedef double                                   DataType;
    typedef int                                      HandleType;
    typedef double                                   CoordinateType;
    typedef int                                      OrdinalType;
    typedef Point<HandleType,CoordinateType>         PointType;
    typedef Teuchos::Comm<OrdinalType>               Communicator_t;
    typedef Teuchos::RCP<const Communicator_t>       RCP_Communicator;
    typedef Teuchos::RCP<Wave>                       RCP_Wave;

  private:

    RCP_Wave wave;
    std::vector<PointType> local_points;

  public:

    Wave_DataTarget(RCP_Wave _wave)
	: wave(_wave)
    { /* ... */ }

    ~Wave_DataTarget()
    { /* ... */ }

    RCP_Communicator get_target_comm()
    {
	return wave->get_comm();
    }

    bool is_field_supported(const std::string &field_name)
    {
	bool return_val = false;

	if (field_name == "DAMPER_FIELD")
	{
	    return_val = true;
	}

	return return_val;
    }

    const Teuchos::ArrayView<PointType> 
    get_target_points(const std::string &field_name)
    {
	Teuchos::ArrayView<PointType> return_view;

	if ( field_name == "DAMPER_FIELD" )
	{
	    local_points.clear();
	    Teuchos::ArrayView<const DataType> local_grid( wave->get_grid() );
	    Teuchos::ArrayView<DataType>::const_iterator grid_it;
	    int n = 0;
	    int global_handle;
	    for (grid_it = local_grid.begin(); 
		 grid_it != local_grid.end();
		 ++grid_it, ++n)
	    {
		global_handle = wave->get_comm()->getRank() *
				local_grid.size() + n;
		local_points.push_back( 
		    PointType( global_handle, *grid_it, 0.0, 0.0) );
	    }
	    return_view = Teuchos::ArrayView<PointType>(local_points);
	}

	return return_view;
    }

    Teuchos::ArrayView<DataType> 
    get_target_data_space(const std::string &field_name)
    {
	Teuchos::ArrayView<DataType> return_view;

	if ( field_name == "DAMPER_FIELD" )
	{
	    return_view = Teuchos::ArrayView<DataType>( wave->set_damping() );
	}

	return return_view;
    }

    void set_global_target_data(const std::string &field_name,
				const DataType &data)
    { /* ... */ }
};

//---------------------------------------------------------------------------//

} // end namespace Coupler

#endif // end wave_target_hpp