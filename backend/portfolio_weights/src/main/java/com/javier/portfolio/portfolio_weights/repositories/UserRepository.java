package com.javier.portfolio.portfolio_weights.repositories;

import org.springframework.data.jpa.repository.JpaRepository;
import com.javier.portfolio.portfolio_weights.models.User;

public interface UserRepository extends JpaRepository<User, Long> {

}